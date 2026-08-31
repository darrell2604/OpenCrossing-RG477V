#include <jni.h>
#include <android/log.h>

namespace {
constexpr char kTag[] = "OpenCrossing";
}

extern "C" JNIEXPORT void JNICALL
Java_com_darrell2604_opencrossing_MainActivity_nativeCreate(JNIEnv*, jobject) {
    __android_log_print(ANDROID_LOG_INFO, kTag,
                        "OpenCrossing native runtime initialised (ARM64 skeleton)");
}
