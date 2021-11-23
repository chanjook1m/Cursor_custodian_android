#include <jni.h>
#include <string>
//#include "Game.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndk_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    //Game game;
    return env->NewStringUTF(hello.c_str());
}