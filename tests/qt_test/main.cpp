#include <iostream>
#include <QList>
#include <QVector>
#include <QTime>
#include <vector>
#include "Element.h"

using namespace std;

#define NUM_ITEMS 1000000
#define NE 3000000
#define NL 100000000

Element::Element(){
	value_one = value_two = value_three = 123456789;
}

Element::Element(int a, int b, int c, std::string d, std::string e, std::string f){
	value_one = a;
	value_two = b;
	value_three = c;
	string_one = d;
	string_two = e;
	string_three = f;
}

int randInt(int low, int high){
	// Random number between low and high
	return qrand() % ((high - 1) - low) + low;
}

int main()
{
	// Seed the random number generator
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	// Fill array with random ints	
	// (These will be used to fill QList and QVector)
	int array[NUM_ITEMS];
	for(unsigned i = 0; i < NUM_ITEMS; i++){
		array[i] = randInt(0,1000000);
	}
	// Store numbers for search later
	int initialNum = array[NUM_ITEMS/4];
	int midNum = array[NUM_ITEMS/2];
	int lateNum = array[(NUM_ITEMS/8)*7];
	int searchNum;

	// Initialize a QTime object
	QTime currentTime;

	cout << "=====[ Tests using large objects (complex) ]=====" << endl;

	// Fill up the QList with Elements
	QList<Element> list;
	currentTime = QTime::currentTime();
	for(unsigned i = 0; i < NUM_ITEMS; i++){
		Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
		list.append(temp);
	}
	// Report outcome to terminal
	cout << "QList insertion: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	currentTime = QTime::currentTime();
	// Run selection sort on list
	QList<Element>::iterator l_it;
	for(unsigned i=0;i<3;i++){
		if(i==0) searchNum = initialNum;
		else if(i==1) searchNum = midNum;
		else if(i==2) searchNum = lateNum;
		for(l_it = list.begin(); l_it != list.end(); l_it++){
			if((*l_it).value_one == searchNum) break;
		}
	}
	cout << "QList search: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	// Fill up the vector with Elements
	QVector<Element> vector;
	currentTime = QTime::currentTime();
	for(unsigned i = 0; i < NUM_ITEMS; i++){
		Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
		vector.push_back(temp);
	}
	// Report outcome to terminal
	cout << "QVector insertion: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	// Fill up the vector with Elements
	QVector<Element> vector_2;
	vector_2.reserve(NUM_ITEMS);
	currentTime = QTime::currentTime();
	for(unsigned i = 0; i < NUM_ITEMS; i++){
		Element temp(array[i],array[i],array[i],"qwertyuiop","asdfghjkl","zxcvbnm");
		vector_2.push_back(temp);
	}
	// Report outcome to terminal
	cout << "QVector insertion (with pre-allocation): " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	currentTime = QTime::currentTime();	
	//Run selection sort on vector
	QVector<Element>::iterator v_it;
	for(unsigned i=0;i<3;i++){
		if(i==0) searchNum = initialNum;
		else if(i==1) searchNum = midNum;
		else if(i==2) searchNum = lateNum;
		for(v_it = vector.begin(); v_it != vector.end(); v_it++){
			if((*v_it).value_one == searchNum) break;
		}
	}
	cout << "QVector search: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;


	// Now running benchmarks as inspired by Collections.cpp...
	cout << "=====[ Tests using ints (simple) ]=====" << endl;
	
	int sum;
	QTime middleTime;

	currentTime = QTime::currentTime();
	QList<int> s;
	for(int i=0;i<NE;i++){
		s.append(i);
	}
	sum = 0;
	middleTime = QTime::currentTime();
	for(int j=0;j<NL;j++){
		foreach(int v, s){
			sum += v;
		}
	}
	cout << "QList insertion: " << currentTime.msecsTo(middleTime) << " ms" << endl;
	cout << "QList iteration: " << middleTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	currentTime = QTime::currentTime();
	QVector<int> qv;
	//qv.reserve(NE);
	for(int i=0;i<NE;i++){
		qv.append(i);
	}
	sum = 0;
	middleTime = QTime::currentTime();
	for(int j=0;j<NL;j++){
		foreach(int v, qv){
			sum += v;
		}
	}
	cout << "QVector insertion: " << currentTime.msecsTo(middleTime) << " ms" << endl;
	cout << "QVector iteration: " << middleTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	currentTime = QTime::currentTime();
	std::vector<int> sv;
	for (int i=0;i<NE;i++){
		sv.push_back(i);
		sum = 0;
		middleTime = QTime::currentTime();
		int sz = sv.size();
		for (int j=0;j<NL;j++){
			for (int i=0;i<sz;i++){
				sum += sv[i];
			}
		}
	}
	cout << "stl::vector insertion: " << currentTime.msecsTo(middleTime) << " ms" << endl;
	cout << "stl::vector iteration: " << middleTime.msecsTo(QTime::currentTime()) << " ms" << endl;

	currentTime = QTime::currentTime();
	int a[NE];
	for (int i=0;i<NE;i++){
		a[i]=i;
		sum = 0;
		for (int j=0;j<NL;j++){
			for (int i=0;i<NE;i++){
				sum += a[i];
			}
		}
	}
	cout << "Plain array total: " << currentTime.msecsTo(QTime::currentTime()) << " ms" << endl;
}
