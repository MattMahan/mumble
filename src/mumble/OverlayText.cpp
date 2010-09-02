/* Copyright (C) 2005-2010, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "OverlayText.h"
#include "Global.h"

BasepointPixmap::BasepointPixmap() :
		qpBasePoint(0, 0) { }

BasepointPixmap::BasepointPixmap(const QPixmap& pm) :
		QPixmap(pm),
		qpBasePoint(0, pm.height()) { }

BasepointPixmap::BasepointPixmap(int w, int h) :
		QPixmap(w, h),
		qpBasePoint(0, h) { }

BasepointPixmap::BasepointPixmap(int w, int h, const QPoint& bp) :
		QPixmap(w, h),
		qpBasePoint(bp) { }

OverlayTextLine::OverlayTextLine(const QString& s, const QFont& f) :
		fEdgeFactor(0.05f),
		qsText(s),
		qfFont(f),
		iCurWidth(-1),
		iCurHeight(-1),
		fBaseliningThreshold(0.5f),
		bElided(false) {
	QFontMetrics fm(f);
	fAscent = static_cast<float>(fm.ascent());
	fDescent = static_cast<float>(fm.descent());
	fEdge = qMax(static_cast<float>(f.pointSizeF()) * fEdgeFactor, 1.0f);
}

BasepointPixmap OverlayTextLine::render(int w, int h, const QColor& col, const QPoint& bp) const {
	BasepointPixmap img(w, h, bp);
	img.fill(Qt::transparent);

	QPainter imgp(&img);
	imgp.setRenderHint(QPainter::Antialiasing);
	imgp.setRenderHint(QPainter::TextAntialiasing);
	imgp.setBackground(QColor(0,0,0,0));
	imgp.setCompositionMode(QPainter::CompositionMode_SourceOver);

	QColor qc(col);
	qc.setAlpha(255);

	imgp.setBrush(qc);
	imgp.setPen(QPen(Qt::black, fEdge, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	imgp.drawPath(qpp);

	imgp.setPen(Qt::NoPen);
	imgp.drawPath(qpp);

	img.iAscent = iroundf(fAscent + 0.5f);
	img.iDescent = iroundf(fDescent + 0.5f);
	return img;
}

BasepointPixmap OverlayTextLine::createPixmap(QColor col) {
	if (qsText.isEmpty()) {
		return BasepointPixmap();
	}

	QRectF qr;
	if (qpp.isEmpty()) {
		qpp.addText(0.0f, fAscent, qfFont, qsText);
		qr = qpp.controlPointRect();

		// fit into (0,0)-based coordinates
		fXCorrection = 0.0f;
		fYCorrection = 0.0f;

		if (qr.left() < fEdge) {
			fXCorrection = fEdge - static_cast<float>(qr.left());
		}

		if (qr.top() < fEdge) {
			fYCorrection = fEdge - static_cast<float>(qr.top());
		}

		QMatrix correction;
		correction.translate(fXCorrection, fYCorrection);
		qpp = correction.map(qpp);
	}

	qr = qpp.controlPointRect();

	return render(
	           iroundf(qr.right() + 2.0f*fEdge + 0.5f),
	           iroundf(qr.bottom() + 2.0f*fEdge + 0.5f),
	           col,
	           QPoint(iroundf(fXCorrection), iroundf(fYCorrection + fAscent))
	       );
}

BasepointPixmap OverlayTextLine::createPixmap(unsigned int maxwidth, unsigned int height, QColor col) {
	float twice_edge = 2.0f * fEdge;

	if (! height || ! maxwidth)
		return BasepointPixmap();

	if (qpp.isEmpty() || iCurWidth > static_cast<int>(maxwidth) || iCurHeight != static_cast<int>(height) || (static_cast<int>(maxwidth) > iCurWidth && bElided)) {
		QFont f = qfFont;
		QFontMetrics fm(f);

		// fit the font into a bounding box with padding
		float ps = static_cast<float>(f.pointSizeF());
		float f_ad = static_cast<float>(fm.ascent() + fm.descent()+1) / ps;

		float pointsize = static_cast<float>(height) / (f_ad + 2.0f*fEdgeFactor);

		if (fEdgeFactor * ps > 1.0f) {
			pointsize = static_cast<float>(height-2) / f_ad;
		}

		if (pointsize <= 0.0f) {
			return BasepointPixmap();
		}

		f.setPointSizeF(pointsize);
		setFont(f);
		fm = QFontMetrics(f);
		twice_edge = 2.0f * fEdge;

		if (!qpp.isEmpty()) {
			qpp = QPainterPath();
		}

		// calculate text metrics for eliding and scaling
		QRectF bb;
		qpp.addText(0.0f, 0.0f, f, qsText);
		bb = qpp.controlPointRect();

		qreal effective_ascent = -bb.top();
		qreal effective_descent = bb.bottom();
		float scale = 1.0f;
		bool keep_baseline = true;
		if (effective_descent > fDescent || effective_ascent > fAscent) {
			qreal scale_ascent = effective_ascent > 0.0f ? fAscent / effective_ascent : 1.0f;
			qreal scale_descent = effective_descent > 0.0f ? fDescent / effective_descent : 1.0f;
			scale = static_cast<float>(qMin(scale_ascent, scale_descent));

			if (scale < fBaseliningThreshold) {
				float text_height = static_cast<float>(bb.height()) + twice_edge;
				scale = static_cast<float>(height) / text_height;
				keep_baseline = false;
			}
		}

		// eliding by previously calculated width
		if ((bb.width()*scale) + twice_edge > maxwidth) {
			int eliding_width = iroundf((static_cast<float>(maxwidth) / scale) - twice_edge);
			QString str = fm.elidedText(qsText, Qt::ElideRight, eliding_width);

			// use ellipsis as shortest possible string
			if (str.trimmed().isEmpty()) {
				str = QString(QChar(0x2026));
			}

			qpp = QPainterPath();
			qpp.addText(0.0f, 0.0f, f, str);
			bb = qpp.controlPointRect();
			bElided = true;
		} else {
			bElided = false;
		}

		// translation to "pixmap space":
		QMatrix correction;
		//  * adjust left edge
		correction.translate(-bb.x() + fEdge, 0.0f);
		//  * scale overly high text (still on baseline)
		correction.scale(scale, scale);

		if (keep_baseline) {
			//  * translate down to baseline
			correction.translate(0.0f, (fAscent + fEdge) / scale);
		} else {
			//  * translate into bounding box
			correction.translate(0.0f, -bb.top() + fEdge);
		}

		qpp = correction.map(qpp);
		iCurWidth = iroundf(bb.width() * scale + 0.5f);
		iCurHeight = height;
	}

	QRectF qr = qpp.controlPointRect();

	return render(
	           iroundf(qr.width() + twice_edge + 0.5f),
	           iroundf(fAscent + fDescent + twice_edge + 0.5f),
	           col,
	           QPoint(0, iroundf(fAscent + fEdge + 0.5f))
	       );
}

void OverlayTextLine::setFont(const QFont& font) {
	qfFont = font;
	qpp = QPainterPath();
	QFontMetrics fm(font);
	fAscent = static_cast<float>(fm.ascent());
	fDescent = static_cast<float>(fm.descent());
	fEdge = qMax(static_cast<float>(font.pointSizeF()) * fEdgeFactor, 1.0f);
}

void OverlayTextLine::setEdge(float ew) {
	fEdge = ew;
	qpp = QPainterPath();
}
