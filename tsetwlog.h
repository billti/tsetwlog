#pragma once
#define WINVER 0x0601 // Support Win7 or later

#include <Windows.h>
#include <TraceLoggingProvider.h>
#include <TraceLoggingActivity.h>
#include <winmeta.h>
#include <node_api.h>

// TraceLogging quickstart: https://msdn.microsoft.com/en-us/library/windows/desktop/dn904627(v=vs.85).aspx
TRACELOGGING_DECLARE_PROVIDER(g_hMyProvider);

namespace demo {
	void InitEtw();
	napi_value LogEvent(napi_env env, napi_callback_info args);
}
