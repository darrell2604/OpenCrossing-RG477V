#include <jni.h>
#include <android/log.h>
#include <GLES3/gl3.h>

#include <cstdint>
#include <string>
#include <vector>

#include "game_runtime.h"
#include "disc_runtime.h"

namespace {
constexpr char kTag[] = "OpenCrossing";
open_crossing::GameRuntime g_runtime;
open_crossing::DiscRuntime g_disc_runtime;
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeSurfaceCreated(JNIEnv*, jobject) {
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "OpenGL ES surface created");
    if (!g_runtime.initialise()) {
        __android_log_print(ANDROID_LOG_ERROR, kTag,
                            "Game runtime initialisation failed");
    }
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

extern "C" JNIEXPORT jboolean JNICALL
Java_com_darrell2604_opencrossing_NativeGameView_nativeLoadDisc(
    JNIEnv* env, jobject, jbyteArray bytes) {
    if (!bytes) {
        __android_log_print(ANDROID_LOG_ERROR, kTag, "Disc load requested with null data");
        return JNI_FALSE;
    }

    const jsize length = env->GetArrayLength(bytes);
    if (length <= 0) {
        __android_log_print(ANDROID_LOG_ERROR, kTag, "Disc load requested with empty data");
        return JNI_FALSE;
    }

    std::vector<std::uint8_t> image(static_cast<std::size_t>(length));
    env->GetByteArrayRegion(bytes, 0, length, reinterpret_cast<jbyte*>(image.data()));
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        __android_log_print(ANDROID_LOG_ERROR, kTag, "Failed to copy disc data from Java");
        return JNI_FALSE;
    }

    std::string error;
    if (!g_disc_runtime.load_image(image, &error)) {
        __android_log_print(ANDROID_LOG_ERROR, kTag,
                            "GameCube disc load failed: %s", error.c_str());
        return JNI_FALSE;
    }

    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "GameCube disc mounted: %s", g_disc_runtime.last_probe().c_str());
    return JNI_TRUE;
}
