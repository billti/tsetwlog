// Based on a version from https://github.com/nodejs/node-gyp/pull/653 to work with Electron
// "Official" node-gyp version at https://github.com/nodejs/node-gyp/blob/master/src/win_delay_load_hook.cc

#ifdef _MSC_VER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
#include <delayimp.h>

#pragma comment(lib, "Shlwapi.lib")

/*
 * This code is needed as the module builds against the node.exe library, but at runtime this is
 * not guaranteed to be the binary name. For example, Electron uses an "electron.exe" host process,
 * are loads Node.js as "node.dll". There are also Node.js forks which may have a different name (e.g. io.js).
 *
 * This means node.exe must be delay loaded, and the hooks below used to return the right module to bind to
 * should it not be named "node.exe".
 *
 * Note: This also means the CRT does need to be linked in :-( , as delayimp.lib must be linked in
 * for delay loading, and unresolved symbols "__load_config_used" and  "___guard_check_icall_fptr"
 * occur without the CRT present. (If you are sure you won't need to load in non-node.exe processes,
 * you can remove this code and also the CRT via /NODEFAULTLIB ).
 */

static FARPROC WINAPI load_exe_hook(unsigned int event, DelayLoadInfo* info) {

	if (event != dliNotePreLoadLibrary)
		return NULL;

	if (lstrcmpiA(info->szDll, "iojs.exe") != 0 &&
		lstrcmpiA(info->szDll, "node.exe") != 0 &&
		lstrcmpiA(info->szDll, "node.dll") != 0)
		return NULL;

	// Get a handle to the current process executable.
	HMODULE processModule = GetModuleHandle(NULL);

	// Get the path to the executable.
	TCHAR processPath[_MAX_PATH];
	GetModuleFileName(processModule, processPath, _MAX_PATH);

	// Get the name of the current executable.
	LPSTR processName = PathFindFileName(processPath);

	// If the current process is node or iojs, then just return the proccess module.
	if (lstrcmpiA(processName, "node.exe") == 0 ||
		lstrcmpiA(processName, "iojs.exe") == 0) {
		return (FARPROC)processModule;
	}

	// If it is another process, attempt to load 'node.dll' from the same directory.
	PathRemoveFileSpec(processPath);
	PathAppend(processPath, "node.dll");

	HMODULE nodeDllModule = GetModuleHandle(processPath);
	if (nodeDllModule != NULL) {
		// This application has a node.dll in the same directory as the executable, use that.
		return (FARPROC)nodeDllModule;
	}

	// Fallback to the current executable, which must statically link to node.lib.
	return (FARPROC)processModule;
}

// See https://docs.microsoft.com/en-us/cpp/build/reference/notification-hooks
extern "C" const PfnDliHook __pfnDliNotifyHook2 = load_exe_hook;

#endif
