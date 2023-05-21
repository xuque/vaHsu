package com.vaHsu.capture;

import android.content.Context;

import com.vaHsu.base.IVideoFrameConsumer;
import com.vaHsu.base.IVideoFrameProvider;
import com.vaHsu.base.VaVideoFrame;
import com.vaHsu.capture.camera.VideoCaptureConfig;
import com.vaHsu.log.VaHsuLog;
import com.vaHsu.render.SurfaceTextureHelper;

import java.lang.ref.WeakReference;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public abstract class IVideoCapture implements IVideoFrameProvider {
    /**
     * This function is used to initialize the camera thread, the android application context, and the
     * capture observer. It will be called only once and before any startCapture() request. The
     * camera thread is guaranteed to be valid until dispose() is called. If the VideoCapturer wants
     * to deliver texture frames, it should do this by rendering on the SurfaceTexture in
     * {@code surfaceTextureHelper}, register itself as a listener, and forward the frames to
     * CaptureObserver.onFrameCaptured(). The caller still has ownership of {@code
     * surfaceTextureHelper} and is responsible for making sure surfaceTextureHelper.dispose() is
     * called. This also means that the caller can reuse the SurfaceTextureHelper to initialize a new
     * VideoCapturer once the previous VideoCapturer has been disposed.
     */
    public abstract void initialize(Context applicationContext, CaptureObserver capturerObserver);

    /**
     * Start capturing frames in a format that is as close as possible to {@code width x height} and
     * {@code framerate}.
     */
    public abstract void startCapture(VideoCaptureConfig config);

    /**
     * Stop capturing. This function should block until capture is actually stopped.
     */
    public abstract void stopCapture() throws InterruptedException;

    public abstract void changeCaptureFormat(int width, int height, int framerate);

    /**
     * Perform any final cleanup here. No more capturing will be done after this call.
     */
    public abstract void dispose();

    /**
     * @return true if-and-only-if this is a screen capturer.
     */
    public abstract boolean isScreencast();

    private List<WeakReference<IVideoFrameConsumer>> mConsumerList = new LinkedList<>();
    @Override
    public int connectToDeliverySink(IVideoFrameConsumer consumer) {
        synchronized (IVideoCapture.class) {
            mConsumerList.add(new WeakReference<>(consumer));
        }
        return 0;
    }

    @Override
    public int disconnectToDeliverySink(IVideoFrameConsumer consumer) {
        synchronized (IVideoCapture.class) {
            Iterator<WeakReference<IVideoFrameConsumer>> iterator = mConsumerList.iterator();
            while (iterator.hasNext()) {
                WeakReference<IVideoFrameConsumer> weakRef = iterator.next();
                IVideoFrameConsumer consumerRef = weakRef.get();
                if (consumerRef == consumer || consumerRef == null) {
                    iterator.remove();
                    VaHsuLog.info("IVideoCapture", "disconnect sink: " + consumer);
                    break;
                }
            }
        }
        return 0;
    }

    public void onFeedFrame(final VaVideoFrame frame) {
        synchronized (IVideoCapture.class) {
            for (WeakReference<IVideoFrameConsumer> consumer : mConsumerList) {
                IVideoFrameConsumer consumerRef = consumer.get();
                if (consumerRef != null) {
                    consumerRef.onDeliveryFrame(frame);
                }
            }
        }
    }
}
