
#ifdef DEBUG
    void dexed_trace(const char *source, const char *fmt, ...);
    #ifdef _MSC_VER
        #define TRACE(fmt, ...) dexed_trace(__FUNCTION__,fmt,##__VA_ARGS__)
    #else
        #define TRACE(fmt, ...) dexed_trace(__PRETTY_FUNCTION__,fmt,##__VA_ARGS__)
    #endif
#else
    #define TRACE(fmt, ...)
#endif