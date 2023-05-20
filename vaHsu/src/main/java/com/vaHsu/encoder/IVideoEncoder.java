package com.vaHsu.encoder;

import com.vaHsu.base.IVideoDataConsumer;
import com.vaHsu.base.IVideoFrameConsumer;
import com.vaHsu.base.IVideoDataProvider;
import com.vaHsu.base.VaVideoFrame;
import com.vaHsu.log.VaHsuLog;

import java.lang.ref.WeakReference;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

public abstract class IVideoEncoder implements IVideoDataProvider, IVideoFrameConsumer {
    public class Settings {
        public final int numberOfCores;
        public final int width;
        public final int height;
        public final int startBitrate; // Kilobits per second.
        public final int maxFramerate;
        public final int numberOfSimulcastStreams;
        public final boolean automaticResizeOn;
        public final Capabilities capabilities;

        public Settings(int numberOfCores, int width, int height, int startBitrate, int maxFramerate,
                        int numberOfSimulcastStreams, boolean automaticResizeOn, Capabilities capabilities) {
            this.numberOfCores = numberOfCores;
            this.width = width;
            this.height = height;
            this.startBitrate = startBitrate;
            this.maxFramerate = maxFramerate;
            this.numberOfSimulcastStreams = numberOfSimulcastStreams;
            this.automaticResizeOn = automaticResizeOn;
            this.capabilities = capabilities;
        }
    }

    public class Capabilities {
        public final boolean lossNotification;

        public Capabilities(boolean lossNotification) {
            this.lossNotification = lossNotification;
        }
    }

    private List<WeakReference<IVideoDataConsumer>> mConsumerList = new LinkedList<>();
    @Override
    public int connectToDeliverySink(IVideoDataConsumer consumer) {
        synchronized (IVideoEncoder.class) {
            mConsumerList.add(new WeakReference<>(consumer));
        }
        return 0;
    }

    @Override
    public int disconnectToDeliverySink(IVideoDataConsumer consumer) {
        synchronized (IVideoEncoder.class) {
            Iterator<WeakReference<IVideoDataConsumer>> iterator = mConsumerList.iterator();
            while (iterator.hasNext()) {
                WeakReference<IVideoDataConsumer> weakRef = iterator.next();
                IVideoDataConsumer consumerRef = weakRef.get();
                if (consumerRef == consumer || consumerRef == null) {
                    iterator.remove();
                    VaHsuLog.info("IVideoEncoder", "disconnect sink: " + consumer);
                    break;
                }
            }
        }
        return 0;
    }

    @Override
    public int onDeliveryFrame(VaVideoFrame frame) {
        return this.encodeFrame();
    }

    public abstract int init();

    public abstract int deInit();

    public abstract int encodeFrame();
}
