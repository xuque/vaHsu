package com.vaHsu.encoder.hard;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;

import java.io.IOException;
import java.nio.ByteBuffer;

import com.vaHsu.base.VaVideoFrame;
import com.vaHsu.encoder.IVideoEncoder;
import com.vaHsu.encoder.VideoEncoderConfig;
import com.vaHsu.log.VaHsuLog;

public class AvcHardEncoder extends IVideoEncoder {
    private static final String TAG = "HardwareCodec";
    private static final String MIME_TYPE = "video/avc";

    private MediaCodec mEncoder;
    private boolean mIsValid;

    public AvcHardEncoder() {

    }

    private int initializeEncoder(VideoEncoderConfig config) {
        if (mIsValid) {
            return 0;
        }

        int ret = 0;
        try {
            MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, config.mWidth, config.mHeight);
            format.setInteger(MediaFormat.KEY_BIT_RATE, config.mFramerate);
            format.setInteger(MediaFormat.KEY_FRAME_RATE, config.mBitrate);
            format.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface);
            format.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, config.mGop);

            mEncoder = MediaCodec.createEncoderByType(MIME_TYPE);
            mEncoder.configure(format, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE);
            mEncoder.start();
            mIsValid = true;
        } catch (IOException e) {
            ret = -1;
            VaHsuLog.error(TAG, "Failed to initialize encoder: " + e.getMessage());
        }
        return ret;
    }

    private int shutdownEncoder() {
        if (!mIsValid) {
            return 0;
        }

        int ret = 0;
        if (mEncoder != null) {
            mEncoder.stop();
            mEncoder.release();
            mEncoder = null;
        }
        return ret;
    }

    private void processFrame(VaVideoFrame frame) {
        byte[] frameData = null; // frame.getBuffer();
        try {
            ByteBuffer[] inputBuffers = mEncoder.getInputBuffers();
            ByteBuffer[] outputBuffers = mEncoder.getOutputBuffers();
            int inputBufferIndex = mEncoder.dequeueInputBuffer(-1);
            if (inputBufferIndex >= 0) {
                ByteBuffer inputBuffer = inputBuffers[inputBufferIndex];
                inputBuffer.clear();
                inputBuffer.put(frameData);
                mEncoder.queueInputBuffer(inputBufferIndex, 0, frameData.length, 0, 0);
            }

            MediaCodec.BufferInfo bufferInfo = new MediaCodec.BufferInfo();
            int outputBufferIndex = mEncoder.dequeueOutputBuffer(bufferInfo, 0);
            while (outputBufferIndex >= 0) {
                ByteBuffer outputBuffer = outputBuffers[outputBufferIndex];
//                handleEncodedData(outputBuffer, bufferInfo);

                mEncoder.releaseOutputBuffer(outputBufferIndex, false);
                outputBufferIndex = mEncoder.dequeueOutputBuffer(bufferInfo, 0);
            }
        } catch (Exception e) {
            VaHsuLog.error(TAG, "Failed to encode frame: " + e.getMessage());
        }
    }

    @Override
    public int init(VideoEncoderConfig config) {
        return initializeEncoder(config);
    }

    @Override
    public int deInit() {
        return shutdownEncoder();
    }

    @Override
    public void encodeFrame(VaVideoFrame frame) {
        processFrame(frame);
    }
}
