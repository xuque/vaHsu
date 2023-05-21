package com.vaHsu.capture.camera.camera1;

import android.content.Context;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.os.SystemClock;
import android.util.Size;
import android.view.SurfaceHolder;

import com.vaHsu.base.VaVideoFrame;
import com.vaHsu.capture.CaptureObserver;
import com.vaHsu.capture.camera.CameraEnumeration;
import com.vaHsu.capture.camera.CameraEnumeration.CaptureFormat;
import com.vaHsu.capture.camera.CameraSession;
import com.vaHsu.capture.camera.CameraVideoCapture;
import com.vaHsu.capture.camera.VideoCaptureConfig;
import com.vaHsu.log.VaHsuLog;
import com.vaHsu.render.SurfaceTextureHelper;
import com.vaHsu.utils.NV21Buffer;

import android.support.annotation.Nullable;

import java.io.IOException;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class Camera1Capture extends CameraVideoCapture implements CameraSession, SurfaceTexture.OnFrameAvailableListener {
    public static String TAG = "Camera1Capture";

    private Context mCtx = null;
    private CaptureObserver mCaptureObserver = null;
    private boolean mFirstFrameReported = false;

    private Camera mCamera;
    private Camera.CameraInfo mCameraInfo;

    private int[] mSurfaceTextureId = new int[1];
    private SurfaceTexture mSurfaceTexture = null;

    private static enum SessionState {
        RUNNING,
        STOPPED
    }
    private SessionState mState;

    @Override
    public void initialize(@Nullable Context context, @Nullable CaptureObserver captureObserver) {
        mCtx = context;
        mCaptureObserver = captureObserver;

        GLES20.glGenTextures(1, mSurfaceTextureId, 0);
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mSurfaceTextureId[0]);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

        mSurfaceTexture = new SurfaceTexture(mSurfaceTextureId[0]);
    }

    @Override
    public void startCapture(@Nullable VideoCaptureConfig config) {
        int cameraId = -1;
        try {
            cameraId = Integer.getInteger(config.getCameraId());
        } catch (SecurityException e) {
            mCaptureObserver.onCameraError(String.format("camera id is invalid, failed: %s", e.getMessage()));
            return;
        }

        if (cameraId == -1) {
            mCaptureObserver.onCameraError(String.format("camera id is invalid: %s", config.getCameraId()));
            return;
        }

        Camera camera = null;
        try {
            camera = Camera.open(cameraId);
        } catch (RuntimeException e) {
            mCaptureObserver.onCameraError(String.format("open camera failed: %s", e.getMessage()));
            return;
        }

        if (camera == null) {
            mCaptureObserver.onCameraError("camera.open returned null");
            return;
        }

        final android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(cameraId, info);

        final CaptureFormat captureFormat;
        try {
            final android.hardware.Camera.Parameters parameters = camera.getParameters();
            captureFormat = findClosestCaptureFormat(parameters, config.getWidth(), config.getHeight(), config.getFramerate());
            final Size pictureSize = findClosestPictureSize(parameters, config.getWidth(), config.getHeight());
            updateCameraParameters(camera, parameters, captureFormat, pictureSize);
        } catch (RuntimeException e) {
            camera.release();
            mCaptureObserver.onCameraError(String.format("camera getParameters failed: %s", e.getMessage()));
            return;
        }

        mSurfaceTexture.setDefaultBufferSize(captureFormat.width, captureFormat.height);
        mSurfaceTexture.setOnFrameAvailableListener(this);

        try {
            camera.setPreviewTexture(mSurfaceTexture);
        } catch (IOException e) {
            mCaptureObserver.onCameraError(String.format("camera setPreviewTexture failed, e: %s", e.getMessage()));
            releaseCamera(camera);
            return;
        }

        try {
            camera.startPreview();
        } catch (RuntimeException e) {
            mCaptureObserver.onCameraError(String.format("start camera preview failed: %s", e.getMessage()));
            releaseCamera(camera);
            return;
        }

        camera.setErrorCallback(new Camera.ErrorCallback() {
            @Override
            public void onError(int error, Camera camera) {
                mCaptureObserver.onCameraError(String.format("camera system callback error: %s", Integer.toString(error)));
            }
        });

        mCamera = camera;
        mCameraInfo = info;
        mCaptureObserver.onCameraOpenSuccess();
    }

    private static CaptureFormat findClosestCaptureFormat(
            android.hardware.Camera.Parameters parameters, int width, int height, int framerate) {
        // Find closest supported format for |width| x |height| @ |framerate|.
        final List<CaptureFormat.FramerateRange> supportedFramerates =
                CameraEnumeration.convertFramerates(parameters.getSupportedPreviewFpsRange());
        VaHsuLog.info(TAG, "Available fps ranges: " + supportedFramerates);

        final CaptureFormat.FramerateRange fpsRange =
                CameraEnumeration.getClosestSupportedFramerateRange(supportedFramerates, framerate);

        final Size previewSize = CameraEnumeration.getClosestSupportedSize(
                CameraEnumeration.convertSizes(parameters.getSupportedPreviewSizes()), width, height);

        return new CaptureFormat(previewSize.getWidth(), previewSize.getHeight(), fpsRange);
    }

    private static Size findClosestPictureSize(
            android.hardware.Camera.Parameters parameters, int width, int height) {
        return CameraEnumeration.getClosestSupportedSize(
                CameraEnumeration.convertSizes(parameters.getSupportedPictureSizes()), width, height);
    }

    private static void updateCameraParameters(android.hardware.Camera camera,
                                               android.hardware.Camera.Parameters parameters, CaptureFormat captureFormat, Size pictureSize) {
        final List<String> focusModes = parameters.getSupportedFocusModes();

        parameters.setPreviewFpsRange(captureFormat.framerate.min, captureFormat.framerate.max);
        parameters.setPreviewSize(captureFormat.width, captureFormat.height);
        parameters.setPictureSize(pictureSize.getWidth(), pictureSize.getHeight());

        if (parameters.isVideoStabilizationSupported()) {
            parameters.setVideoStabilization(true);
        }
        if (focusModes.contains(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO)) {
            parameters.setFocusMode(android.hardware.Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }
        camera.setParameters(parameters);
    }

    private int getFrameOrientation(Camera.CameraInfo info) {
        int rotation = CameraSession.getDeviceOrientation(mCtx);
        if (info.facing == android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK) {
            rotation = 360 - rotation;
        }
        return (info.orientation + rotation) % 360;
    }

    private void releaseCamera(Camera camera) {
        if (camera != null) {
            camera.release();
        }

        mCamera = null;
        mFirstFrameReported = false;
    }

    @Override
    public void stopCapture() {
        mSurfaceTexture.setOnFrameAvailableListener(null);
        releaseCamera(mCamera);
    }

    @Override
    public void changeCaptureFormat(int width, int height, int framerate) {

    }

    @Override
    public void dispose() {

    }

    @Override
    public boolean isScreencast() {
        return false;
    }

    @Override
    public void switchCamera(CameraSwitchHandler switchEventsHandler) {

    }

    @Override
    public void enableFaceCamera(boolean bEnable) {

    }

    @Override
    public void onFrameAvailable(SurfaceTexture surfaceTexture) {

    }
}