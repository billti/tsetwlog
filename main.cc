#include "tsetwlog.h"
#include <TraceLoggingProvider.h>
#include <TraceLoggingActivity.h>
#include <winmeta.h>
#include <vector>

using namespace std;

namespace demo {

	struct MethodDef {
		const char *name;
		napi_callback cb;
	};

	napi_value Init(napi_env env, napi_value exports) {
		napi_status status;
		napi_value fn;

		InitEtw();

		// TODO: napi_add_env_cleanup_hook to CleanupEtw (only in >= Node.js 10.2.0)
		// Should call: TraceLoggingUnregister(g_hMyProvider);

		vector<MethodDef> methods{
			{"logEvent", LogEvent},
			{"logStartCommand", LogStartCommand},
			{"logStopCommand", LogStopCommand}
		};

		for (MethodDef& method : methods)
		{
			status = napi_create_function(env, method.name, 0, method.cb, nullptr, &fn);
			if (status != napi_ok) return nullptr;

			status = napi_set_named_property(env, exports, method.name, fn);
			if (status != napi_ok) return nullptr;
		}

		return exports;
	}

	NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
}  // namespace demo
