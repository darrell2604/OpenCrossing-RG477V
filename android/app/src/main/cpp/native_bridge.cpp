#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>

namespace {
constexpr char kTag[] = "OpenCrossing";
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeSurfaceCreated(JNIEnv*, jobject) {
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "OpenGL ES surface created");
    glClearColor(0.08f, 0.12f, 0.10f, 1.0f);
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeSurfaceChanged(
    JNIEnv*, jobject, jint width, jint height) {
    glViewport(0, 0, width, height);
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "Surface resized to %dx%d", width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeDrawFrame(JNIEnv*, jobject) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
