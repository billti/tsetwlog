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

		// Requires Node.js 10.2 or later. See https://nodejs.org/dist/latest-v10.x/docs/api/n-api.html#n_api_napi_add_env_cleanup_hook
		status = napi_add_env_cleanup_hook(env, CleanupEtw, nullptr);

		vector<MethodDef> methods{
			{"logEvent", LogEvent},
			{"logStartCommand", LogStartCommand},
			{"logStopCommand", LogStopCommand},
			{"logStartUpdateProgram", LogStartUpdateProgram},
			{"logStopUpdateProgram", LogStopUpdateProgram},
			{"logStartUpdateGraph", LogStartUpdateGraph},
			{"logStopUpdateGraph", LogStopUpdateGraph},
			{"logStartResolveModule", LogStartResolveModule},
			{"logStopResolveModule", LogStopResolveModule},
			{"logStartParseSourceFile", LogStartParseSourceFile},
			{"logStopParseSourceFile", LogStopParseSourceFile},
			{"logStartReadFile", LogStartReadFile},
			{"logStopReadFile", LogStopReadFile},
			{"logStartBindFile", LogStartBindFile},
			{"logStopBindFile", LogStopBindFile},
			{"logStartScheduledOperation", LogStartScheduledOperation},
			{"logStopScheduledOperation", LogStopScheduledOperation},
			{"logErrEvent", LogErrEvent},
			{"logInfoEvent", LogInfoEvent},
			{"logPerfEvent", LogPerfEvent}
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
