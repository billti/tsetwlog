#pragma once
#define WINVER 0x0601 // Support Win7 or later
#define WIN32_LEAN_AND_MEAN

// The below intrinsics are needed on debug builds currently
// See https://docs.microsoft.com/en-us/cpp/intrinsics/intrinsics-available-on-all-architectures
#include <string.h>
#pragma intrinsic(memset, wcslen)

#include <Windows.h>
#include <TraceLoggingProvider.h>
#include <TraceLoggingActivity.h>
#include <winmeta.h>
#include <node_api.h>

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
