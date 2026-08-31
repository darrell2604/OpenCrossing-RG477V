#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>

#include "game_runtime.h"

namespace {
constexpr char kTag[] = "OpenCrossing";
open_crossing::GameRuntime g_runtime;
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeSurfaceCreated(JNIEnv*, jobject) {
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "OpenGL ES surface created");
    g_runtime.initialise();
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeSurfaceChanged(
    JNIEnv*, jobject, jint width, jint height) {
    g_runtime.resize(width, height);
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "Surface resized to %dx%d", width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeDrawFrame(JNIEnv*, jobject) {
    g_runtime.frame();
}
