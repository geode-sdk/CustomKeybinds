#include <Geode/platform/cplatform.h>

#if defined(GEODE_IS_WINDOWS)
#include <Windows.h>

inline double getCurrentTimestamp() {
    static LARGE_INTEGER frequency = [] {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);
        return freq;
    }();

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return static_cast<double>(now.QuadPart) / static_cast<double>(frequency.QuadPart);
}
#elif defined(GEODE_IS_ANDROID)
#include <Geode/cocos/platform/android/jni/JniHelper.h>

inline double getCurrentTimestamp() {
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, "android/os/SystemClock", "uptimeMillis", "()J")) {
        auto ret = t.env->CallStaticLongMethod(t.classID, t.methodID) / 1000.0;
        t.env->DeleteLocalRef(t.classID);
        return ret;
    }
    return 0.0;
}
#elif defined(GEODE_IS_MACOS) || defined(GEODE_IS_IOS)
#include <objc/message.h>
#include <objc/runtime.h>

inline double getCurrentTimestamp() {
    id processInfo = reinterpret_cast<id(*)(Class, SEL)>(objc_msgSend)(objc_getClass("NSProcessInfo"), sel_registerName("processInfo"));
    return reinterpret_cast<double(*)(id, SEL)>(objc_msgSend)(processInfo, sel_registerName("systemUptime"));
}
#endif