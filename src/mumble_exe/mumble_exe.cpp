/* Copyright (C) 2011-2013, Benjamin Jemlich <pcgod@users.sourceforge.net>
   Copyright (C) 2013, Mikkel Krautz <mikkel@krautz.dk>

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

#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>

#include <string>

typedef int (*DLL_MAIN)(HINSTANCE, HINSTANCE, LPSTR, int);
#ifdef DEBUG
typedef int (*DLL_DEBUG_MAIN)(int, char **);
#endif

// Alert shows a fatal error dialog and waits for the user to click OK.
static void Alert(LPCWSTR title, LPCWSTR msg) {
	MessageBox(NULL, msg, title, MB_OK|MB_ICONERROR);
}

// GetExecutableDirPath returns the directory that
// mumble.exe resides in.
static std::wstring GetExecutableDirPath() {
	wchar_t path[MAX_PATH];

	if (GetModuleFileNameW(NULL, path, MAX_PATH) == 0)
		return std::wstring();

	if (!PathRemoveFileSpecW(path))
		return std::wstring();

	std::wstring exe_path(path);
	return exe_path.append(L"\\");
}

// ConfigureEnvironment prepares mumble.exe's environment to
// run mumble_app.dll.
static bool ConfigureEnvironment() {
	std::wstring exe_path = GetExecutableDirPath();

	// Remove the current directory from the DLL search path.
	if (!SetDllDirectoryW(L""))
		return false;

	// Set mumble.exe's directory as the current working directory.
	if (!SetCurrentDirectoryW(exe_path.c_str()))
		return false;

	return true;
}

// GetAbsoluteMumbleAppDllPath returns the absolute path to
// mumble_app.dll - the DLL containing the Mumble client
// application code.
static std::wstring GetAbsoluteMumbleAppDllPath() {
	std::wstring exe_path = GetExecutableDirPath();
	if (exe_path.empty())
		return std::wstring();

	std::wstring abs_dll_path(exe_path);
	abs_dll_path.append(L"\\");
	abs_dll_path.append(L"mumble_app.dll");
	return abs_dll_path;
}

#ifdef DEBUG
int main(int argc, char **argv) {
	if (!ConfigureEnvironment()) {
		Alert(L"Mumble Launcher Error -1", L"Unable to configure environment.");
		return -1;
	}

	std::wstring abs_dll_path = GetAbsoluteMumbleAppDllPath();
	if (abs_dll_path.empty()) {
		Alert(L"Mumble Launcher Error -2", L"Unable to find the absolute path of mumble_app.dll.");
		return -2;
	}

	HMODULE dll = LoadLibraryExW(abs_dll_path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!dll) {
		Alert(L"Mumble Launcher Error -3", L"Failed to load mumble_app.dll.");
		return -3;
	}

	DLL_DEBUG_MAIN entry_point = reinterpret_cast<DLL_DEBUG_MAIN>(GetProcAddress(dll, "main"));
	if (!entry_point) {
		Alert(L"Mumble Launcher Error -4", L"Unable to find expected entry point ('main') in mumble_app.dll.");
		return -4;
	}

	int rc = entry_point(argc, argv);

	return rc;
}
#endif  // DEBUG

int APIENTRY wWinMain(HINSTANCE instance, HINSTANCE prevInstance, wchar_t *cmdArg, int cmdShow) {
	if (!ConfigureEnvironment()) {
		Alert(L"Mumble Launcher Error -1", L"Unable to configure environment.");
		return -1;
	}

	std::wstring abs_dll_path = GetAbsoluteMumbleAppDllPath();
	if (abs_dll_path.empty()) {
		Alert(L"Mumble Launcher Error -2", L"Unable to find the absolute path of mumble_app.dll.");
		return -2;
	}

	HMODULE dll = LoadLibraryExW(abs_dll_path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (!dll) {
		Alert(L"Mumble Launcher Error -3", L"Failed to load mumble_app.dll.");
		return -3;
	}

	DLL_MAIN entry_point = reinterpret_cast<DLL_MAIN>(GetProcAddress(dll, "MumbleMain"));
	if (!entry_point) {
		Alert(L"Mumble Launcher Error -4", L"Unable to find expected entry point ('MumbleMain') in mumble_app.dll.");
		return -4;
	}

	(void) cmdArg;
	int rc = entry_point(instance, prevInstance, NULL, cmdShow);

	return rc;
}
