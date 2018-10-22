#include "tsetwlog.h"

/*
 * To keep size & dependencies down, this build has some "interesting" configuration.
 *  - This builds with the /NODEFAULTLIB option (i.e. CRT is unavailable) to exclude all the default library dependencies
 *  - Which means needing to define DllMain to do the work of the CRT version and enter it directly (/ENTRY:DllMain)
 *  - This means basics such as new & delete, or std::string & std::vector need to be implemented if needed
 *  - Also, don't allocate over a page (4kb) of locals in a function, else you'll get an unresolved external symbol for _chkstk
 *    - See https://stackoverflow.com/questions/7420927/unresolved-symbols-when-linking-without-c-runtime-with-vs2010
 *  - This also requires knowing how some of the funky ETW and Node.js macros work to avoid potentical issues:
 *    - See details on NAPI_MODULE below. This required mimicing in the process attach event in DllMain to initialize
 *    - ETW initialization (TRACELOGGING_DEFINE_PROVIDER) _appears_ to be a constant expression, (so hopefully does require startup initialization)
 *  - Exceptions and runtime type information are also disabled for simplicity and size.
 *
 * Result: A skeleton native module compiled is around 15kb, instead of around 100kb when statically linking in the CRT.
 */

namespace tsetwlog {

	napi_value Init(napi_env env, napi_value exports) {
		InitEtw();
		
		auto register_fn = [&](const char *name, napi_callback cb) {
			napi_value fn;
			napi_status status;
			status = impl_napi_create_function(env, name, 0, cb, nullptr, &fn);
			if (status == napi_ok) {
				status = impl_napi_set_named_property(env, exports, name, fn);
			}
			return status == napi_ok;
		};

		// Sort-circuiting evaluation for any failures
		// Note: napi_add_env_cleanup_hook requires Node.js 10.2 or later. See https://nodejs.org/dist/latest-v10.x/docs/api/n-api.html#n_api_napi_add_env_cleanup_hook
		bool success = impl_napi_add_env_cleanup_hook(env, CleanupEtw, nullptr) == napi_ok &&
			register_fn("logEvent", LogEvent) &&
			register_fn("logStartCommand", LogStartCommand) &&
			register_fn("logStopCommand", LogStopCommand) &&
			register_fn("logStartUpdateProgram", LogStartUpdateProgram) &&
			register_fn("logStopUpdateProgram", LogStopUpdateProgram) &&
			register_fn("logStartUpdateGraph", LogStartUpdateGraph) &&
			register_fn("logStopUpdateGraph", LogStopUpdateGraph) &&
			register_fn("logStartResolveModule", LogStartResolveModule) &&
			register_fn("logStopResolveModule", LogStopResolveModule) &&
			register_fn("logStartParseSourceFile", LogStartParseSourceFile) &&
			register_fn("logStopParseSourceFile", LogStopParseSourceFile) &&
			register_fn("logStartReadFile", LogStartReadFile) &&
			register_fn("logStopReadFile", LogStopReadFile) &&
			register_fn("logStartBindFile", LogStartBindFile) &&
			register_fn("logStopBindFile", LogStopBindFile) &&
			register_fn("logStartScheduledOperation", LogStartScheduledOperation) &&
			register_fn("logStopScheduledOperation", LogStopScheduledOperation) &&
			register_fn("logErrEvent", LogErrEvent) &&
			register_fn("logInfoEvent", LogInfoEvent) &&
			register_fn("logPerfEvent", LogPerfEvent);

		if (!success) return nullptr; // TODO: Set exception?

		return exports;
	}

	// NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

	/* The above expands in the preprocessor to the below. Allocating the registration function
	 * directly in the C-runtime initialization section (".CRT$XCU") is a little "awkward", and 
	 * does make it hard to build native add-ons that don't link against the CRT.

	extern "C" {
		static napi_module _module = 
		{ 
			1, 
			0, 
			"c:\\src\\github\\tsetwlog\\main.cc", 
			Init, 
			"tsetwlog", 
			0, 
	        {0}, 
		}; 
		static void __cdecl _register_tsetwlog(void); 
		__declspec(dllexport, allocate(".CRT$XCU")) void(__cdecl * _register_tsetwlog_)(void) = _register_tsetwlog;
		static void __cdecl _register_tsetwlog(void) { 
			napi_module_register(&_module);
		}
	}

	*/

	// Do the work the macro would if we were using the C-runtime library to initialize (avoiding about 75KB of binary)
	extern "C" {
		static napi_module _module;

		bool WINAPI DllMain(
			_In_ HINSTANCE hinstDLL,
			_In_ DWORD     fdwReason,
			_In_ LPVOID    lpvReserved) {

			if (fdwReason == DLL_PROCESS_ATTACH)
			{
				LoadNapiFunctions();
				_module = {
					NAPI_MODULE_VERSION,
					0,
					__FILE__,
					Init,
					"tsetwlog",
					0,
					{0},
				};

				impl_napi_module_register(&_module);
				return true;
			}
		}
	}
}  // namespace tsetwlog
