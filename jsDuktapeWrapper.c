#include "jsDuktapeWrapper.h"
#include "duktape.h"

void myFatal (duk_context *ctx, duk_errcode_t code, const char *msg) {
	printf("Error code: %d\n", code);
	printf("Error message: %s\n", msg);
	exit(-1);
}

duk_ret_t native_request_send(duk_context *ctx) {
	const char *method = duk_require_string(ctx, 0);
	const char *url = duk_require_string(ctx, 1);
	const char *data = duk_get_string(ctx, 2);

	(void) duk_get_global_string(ctx, "JNIEnv");
	JNIEnv *env = (JNIEnv *)duk_require_pointer(ctx, -1);
	jclass duktape_wrapper_jclass =
			(*env)->FindClass(env, "fi/helsinki/cs/iot/hub/jsengine/DuktapeJavascriptEngineWrapper");
	const char *signature =
			"(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;";
	jmethodID perform_http_jmethodID =
			(*env)->GetStaticMethodID(env, duktape_wrapper_jclass, "performJavaHttpRequest", signature);
	jstring jmethod = (*env)->NewStringUTF(env, method);
	jstring jurl = (*env)->NewStringUTF(env, url);
	jstring jdata = NULL;
	if (data != NULL) {
		jdata = (*env)->NewStringUTF(env, data);
	}
	jstring json_response_jstring =
			(jstring) (*env)->CallStaticObjectMethod(env, duktape_wrapper_jclass, perform_http_jmethodID, jmethod, jurl, jdata);
	const char *response = (*env)->GetStringUTFChars(env, json_response_jstring, 0);
	duk_pop(ctx);  // pop global

	//Now just put the response on the stack
	duk_idx_t obj_idx = duk_push_object(ctx);
	duk_push_string(ctx, response);
	duk_put_prop_string(ctx, obj_idx, "response");
	(*env)->ReleaseStringUTFChars(env, json_response_jstring, response);
	return 1;
}

JNIEXPORT jboolean JNICALL Java_fi_helsinki_cs_iot_hub_javascript_DuktapeJavascriptEngineWrapper_runScript
(JNIEnv *env, jobject thisObj, jstring script) {
	//duk_context *ctx = duk_create_heap_default();
	duk_context *ctx = duk_create_heap(NULL, NULL, NULL, NULL, &myFatal);
	if (!ctx) {
		printf("Failed to create a Duktape heap.\n");
		return JNI_FALSE;
	}

	//I need access to the JNIEnv in my native_request_send
	duk_push_global_object(ctx);
	duk_push_pointer(ctx, env);
	duk_put_prop_string(ctx, -2, "JNIEnv");
	duk_pop(ctx);  /* pop global */

	duk_push_global_object(ctx);
	duk_push_c_function(ctx, native_request_send, 2);
	duk_put_prop_string(ctx, -2, "native_request_send");
	duk_pop(ctx);  /* pop global */


	//TODO the httpRequest.js must be added to a config file
	if (duk_peval_file(ctx, "jni/httpRequest.js") != 0) {
		/* Use duk_safe_to_string() to convert error into string.  This API
		 * call is guaranteed not to throw an error during the coercion.
		 */
		printf("Script error: %s\n", duk_safe_to_string(ctx, -1));
	}
	duk_pop(ctx);

	const char *nativeScript = (*env)->GetStringUTFChars(env, script, 0);
	jboolean success = JNI_FALSE;
	//nativeScript = "print('program code'); syntax error here=";
	if (duk_peval_string(ctx, nativeScript) != 0) {
		printf("compile failed: %s\n", duk_safe_to_string(ctx, -1));
	} else {
		//printf("program calls\n");
		//duk_call(ctx, 0);      /* [ func ] -> [ result ] */
		printf("program result: %s\n", duk_get_string(ctx, -1));
		success = JNI_TRUE;
	}

	//duk_eval_string(ctx, nativeScript);
	duk_pop(ctx);  /* pop eval result */

	duk_destroy_heap(ctx);

	(*env)->ReleaseStringUTFChars(env, script, nativeScript);
	return success;
}
