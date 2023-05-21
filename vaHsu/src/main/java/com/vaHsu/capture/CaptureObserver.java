package com.vaHsu.capture;


public interface CaptureObserver {
    void onCameraOpenSuccess();
    void onCameraStopped();

    void onCameraError(String error);
    void onCameraDisconnected();
    void onCameraClosed();
    void onFirstFrameAvailable();
}
