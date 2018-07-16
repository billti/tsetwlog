#include "tsetwlog.h"
#include <TraceLoggingProvider.h>
#include <TraceLoggingActivity.h>
#include <winmeta.h>


namespace demo {

	napi_value Init(napi_env env, napi_value exports) {
		napi_status status;
		napi_value fn;

		InitEtw();

		// TODO: napi_add_env_cleanup_hook to CleanupEtw (only in >= Node.js 10.2.0)

		status = napi_create_function(env, "logEvent", 0, LogEvent, nullptr, &fn);
		if (status != napi_ok) return nullptr;

		status = napi_set_named_property(env, exports, "logEvent", fn);
		if (status != napi_ok) return nullptr;
		return exports;
	}

	NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
}  // namespace demo
