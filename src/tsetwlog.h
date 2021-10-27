#pragma once
#define WINVER 0x0601 // Support Win7 or later
#define UNICODE

// The below intrinsics are needed on debug builds currently
// See https://docs.microsoft.com/en-us/cpp/intrinsics/intrinsics-available-on-all-architectures
#include <string.h>
#pragma intrinsic(memset, wcslen)

#include <Windows.h>
#include <TraceLoggingProvider.h>
#include <TraceLoggingActivity.h>
#include <evntprov.h>
#include <winmeta.h>
#include <node_api.h>

bool LoadNapiFunctions();

// Declare or define the NAPI method pointers?
#ifdef NAPI_IMPL
  #define DECL_NAPI_IMPL(fn_name) decltype(&fn_name) p##fn_name
#else
  #define DECL_NAPI_IMPL(fn_name) extern decltype(&fn_name) p##fn_name
#endif


DECL_NAPI_IMPL(napi_module_register);
DECL_NAPI_IMPL(napi_create_function);
DECL_NAPI_IMPL(napi_set_named_property);
DECL_NAPI_IMPL(napi_get_cb_info);
DECL_NAPI_IMPL(napi_typeof);
DECL_NAPI_IMPL(napi_get_value_string_utf16);
//DECL_NAPI_IMPL(napi_add_env_cleanup_hook); // Not available on Node.js 8

constexpr UINT64 TSSERVER_KEYWORD_PERF = 0x01;

// TraceLogging quickstart: https://msdn.microsoft.com/en-us/library/windows/desktop/dn904627(v=vs.85).aspx
TRACELOGGING_DECLARE_PROVIDER(g_hMyProvider);
// The above basically becomes: extern const TraceLoggingHProvider g_hMyProvider;

namespace tsetwlog {
	void InitEtw();
	void CleanupEtw(void *arg);
	napi_value LogEvent(napi_env env, napi_callback_info args);
	napi_value LogErrEvent(napi_env env, napi_callback_info args);
	napi_value LogPerfEvent(napi_env env, napi_callback_info args);
	napi_value LogInfoEvent(napi_env env, napi_callback_info args);
	napi_value LogStartCommand(napi_env env, napi_callback_info args);
	napi_value LogStopCommand(napi_env env, napi_callback_info args);
	napi_value LogStartUpdateProgram(napi_env env, napi_callback_info args);
	napi_value LogStopUpdateProgram(napi_env env, napi_callback_info args);
	napi_value LogStartUpdateGraph(napi_env env, napi_callback_info args);
	napi_value LogStopUpdateGraph(napi_env env, napi_callback_info args);
	napi_value LogStartResolveModule(napi_env env, napi_callback_info args);
	napi_value LogStopResolveModule(napi_env env, napi_callback_info args);
	napi_value LogStartParseSourceFile(napi_env env, napi_callback_info args);
	napi_value LogStopParseSourceFile(napi_env env, napi_callback_info args);
	napi_value LogStartReadFile(napi_env env, napi_callback_info args);
	napi_value LogStopReadFile(napi_env env, napi_callback_info args);
	napi_value LogStartBindFile(napi_env env, napi_callback_info args);
	napi_value LogStopBindFile(napi_env env, napi_callback_info args);
	napi_value LogStartScheduledOperation(napi_env env, napi_callback_info args);
	napi_value LogStopScheduledOperation(napi_env env, napi_callback_info args);
} // namespace tsetwlog


// The below helpers are to convert the ActivityId GUID passed in the request string.

// Take one hex digit and returns the 0 - 15 value for it (or 0xFF for failure)
inline BYTE HexCodeToNibble(wchar_t code) {
	// Too high or low (all letters and digits are within this range)
	if (code < '0' || code > 'z') return '\xFF';
	// Digits come first (and it is >= '0' at this point)
	if (code <= '9') return code - '0';
	// Convert to uppercase (lowercase is 0x20 higher)
	if (code >= 'a') code = code - 0x20;
	// Ensure it is within HEX range.
	if (code < 'A' || code > 'F') return '\xFF';
	// Must be in the range 'A' to 'F' at this point
	return code - 'A' + 10;
}

// Convert a string (strictly) of the format: {ac4e9dd1-3a7a-5022-fc37-f2394bc4f09e} to a GUID
inline bool StringToGuid(const wchar_t* guid_str, GUID& result) {
	// GUID struct is : ULONG, USHORT, USHORT, UCHAR[8]
	// Last 8 bytes will be in order shown in the string, e.g. 0xfc, 0x37, 0xf2, 0x39...

	int nibble = 0;
	BYTE char_value = 0;
	memset(&result, 0, sizeof(GUID));

	for (int pos = 0; pos <= 37; pos++) {
		switch (pos) {
		case 0:
			if (guid_str[pos] != '{') return false;
			break;
		case 37:
			if (guid_str[pos] != '}') {
				return false;
			}
			else {
				// Got to the final char correctly
				return true;
			}
		case 9:
		case 14:
		case 19:
		case 24:
			if (guid_str[pos] != '-') return false;
			break;
		default:
			// Below will also return 0xFF on null (i.e. early end of string)
			char_value = HexCodeToNibble(guid_str[pos]);
			if (char_value == '\xFF') return false;

			// Initial value only true for the last 8 bytes.
			int bytePos = nibble / 2;

			if (nibble < 8) {
				// First ULONG. Bytes 0 to 3 go to positions 3 to 0 (as little endian)
				bytePos = 3 - (nibble / 2);
			} 
			else if (nibble < 12) {
				// First USHORT. Bytes 4 & 5 go to positions 5 & 4 (as little endian)
				bytePos = (nibble / 2 == 4) ? 5 : 4;
			} 
			else if (nibble < 16) {
				// Second USHORT. Bytes 6 & 7 go to positions 7 & 6 (as little endian)
				bytePos = (nibble / 2 == 6) ? 7 : 6;
			}
			// Else final 8 bytes. Initial byte position is correct.

			// For the first nibbles (even positions), shift up to the high end of the byte
			if (nibble % 2 == 0) char_value <<= 4;
			reinterpret_cast<BYTE*>(&result)[bytePos] |= char_value;
			++nibble;
			break;
		}
	}
	// Should never get here due to return true or false at index 37.
	return false;
}