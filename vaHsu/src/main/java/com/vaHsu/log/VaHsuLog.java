package com.vaHsu.log;

import static com.vaHsu.VaHsuTypes.VaLogLevel.VA_LOG_LEVEL_DEBUG;
import static com.vaHsu.VaHsuTypes.VaLogLevel.VA_LOG_LEVEL_ERROR;
import static com.vaHsu.VaHsuTypes.VaLogLevel.VA_LOG_LEVEL_INFO;
import static com.vaHsu.VaHsuTypes.VaLogLevel.VA_LOG_LEVEL_TRACE;
import static com.vaHsu.VaHsuTypes.VaLogLevel.VA_LOG_LEVEL_WARN;

public class VaHsuLog {
    private static IVaHsuLog mLogCb = null;
    private static VaHsuLog mInstance = null;
    public static VaHsuLog instance() {
        if (mInstance == null) {
            synchronized (VaHsuLog.class) {
                mInstance = new VaHsuLog();
            }
        }
        return mInstance;
    }

    private VaHsuLog() {

    }

    public static void setLogCallback(IVaHsuLog callback) {
        synchronized (VaHsuLog.class) {
            mLogCb = callback;
        }
    }

    public static void onLogMsgCallback(int level, String tag, String msg) {
        synchronized (VaHsuLog.class) {
            if (mLogCb != null) {
                mLogCb.onVaLog(level, tag, msg);
            }
        }
    }

    private static int logLevel = VA_LOG_LEVEL_TRACE;

    public static void setLogLevel(int level) {
        logLevel = level;
    }

    public static void trace(String tag, String format, Object... args) {
        logM(VA_LOG_LEVEL_TRACE, tag, format, args);
    }

    public static void trace(String tag, String text) {
        logM(VA_LOG_LEVEL_TRACE, tag, text);
    }

    public static void debug(String tag, String format, Object... args) {
        logM(VA_LOG_LEVEL_DEBUG, tag, format, args);
    }

    public static void debug(String tag, String text) {
        logM(VA_LOG_LEVEL_DEBUG, tag, text);
    }

    public static void info(String tag, String format, Object... args) {
        logM(VA_LOG_LEVEL_INFO, tag, format, args);
    }

    public static void info(String tag, String text) {
        logM(VA_LOG_LEVEL_INFO, tag, text);
    }

    public static void warn(String tag, String format, Object... args) {
        logM(VA_LOG_LEVEL_WARN, tag, format, args);
    }

    public static void warn(String tag, String text) {
        logM(VA_LOG_LEVEL_WARN, tag, text);
    }

    public static void error(String tag, String format, Object... args) {
        logM(VA_LOG_LEVEL_ERROR, tag, format, args);
    }

    public static void error(String tag, String text) {
        logM(VA_LOG_LEVEL_ERROR, tag, text);
    }

    private static void logM(int level, String tag, String text) {
        if (level >= logLevel) {
            onLogMsgCallback(level, tag, text);
        }
    }

    private static void logM(int level, String tag, String format, Object... args) {
        if (level >= logLevel) {
            onLogMsgCallback(level, tag, String.format(format, args));
        }
    }
}
