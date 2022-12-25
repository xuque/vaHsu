package com.vaHsu.log;

public interface IVaHsuLog {
    /**
     * Log callback interface<br/>
     * <p>Implement this interface for registering a log callback </p>
     *
     * @param level Level {@link com.vaHsu.VaHsuTypes}
     * @param tag   Tag
     * @param msg   Message
     */
    public void onVaLog(int level, String tag, String msg);
}
