package com.vaHsu.base;

import android.graphics.Matrix;
import android.opengl.GLES11Ext;
import android.opengl.GLES20;

import java.nio.ByteBuffer;

public class VaVideoFrame {
    public interface Buffer {
        /**
         * Resolution of the buffer in pixels.
         */
        int getWidth();
        int getHeight();

        /**
         * Returns a memory-backed frame in I420 format. If the pixel data is in another format, a
         * conversion will take place. All implementations must provide a fallback to I420 for
         * compatibility with e.g. the internal WebRTC software encoders.
         */
        I420Buffer toI420();

        void retain();
        void release();

        /**
         * Crops a region defined by |cropx|, |cropY|, |cropWidth| and |cropHeight|. Scales it to size
         * |scaleWidth| x |scaleHeight|.
         */
        Buffer cropAndScale(
                int cropX, int cropY, int cropWidth, int cropHeight, int scaleWidth, int scaleHeight);
    }

    /**
     * Interface for I420 buffers.
     */
    public interface I420Buffer extends Buffer {
        /**
         * Returns a direct ByteBuffer containing Y-plane data. The buffer capacity is at least
         * getStrideY() * getHeight() bytes. The position of the returned buffer is ignored and must
         * be 0. Callers may mutate the ByteBuffer (eg. through relative-read operations), so
         * implementations must return a new ByteBuffer or slice for each call.
         */
        ByteBuffer getDataY();
        /**
         * Returns a direct ByteBuffer containing U-plane data. The buffer capacity is at least
         * getStrideU() * ((getHeight() + 1) / 2) bytes. The position of the returned buffer is ignored
         * and must be 0. Callers may mutate the ByteBuffer (eg. through relative-read operations), so
         * implementations must return a new ByteBuffer or slice for each call.
         */
        ByteBuffer getDataU();

        /**
         * Returns a direct ByteBuffer containing V-plane data. The buffer capacity is at least
         * getStrideV() * ((getHeight() + 1) / 2) bytes. The position of the returned buffer is ignored
         * and must be 0. Callers may mutate the ByteBuffer (eg. through relative-read operations), so
         * implementations must return a new ByteBuffer or slice for each call.
         */
        ByteBuffer getDataV();

        int getStrideY();
        int getStrideU();
        int getStrideV();
    }

    /**
     * Interface for buffers that are stored as a single texture, either in OES or RGB format.
     */
    public interface TextureBuffer extends Buffer {
        enum Type {
            OES(GLES11Ext.GL_TEXTURE_EXTERNAL_OES),
            RGB(GLES20.GL_TEXTURE_2D);

            private final int glTarget;

            private Type(final int glTarget) {
                this.glTarget = glTarget;
            }

            public int getGlTarget() {
                return glTarget;
            }
        }

        Type getType();
        int getTextureId();

        /**
         * Retrieve the transform matrix associated with the frame. This transform matrix maps 2D
         * homogeneous coordinates of the form (s, t, 1) with s and t in the inclusive range [0, 1] to
         * the coordinate that should be used to sample that location from the buffer.
         */
        Matrix getTransformMatrix();
    }

    private final Buffer buffer;
    private final int rotation;
    private final long timestampNs;

    /**
     * Constructs a new VideoFrame backed by the given {@code buffer}.
     *
     * @note Ownership of the buffer object is tranferred to the new VideoFrame.
     */
    public VaVideoFrame(Buffer buffer, int rotation, long timestampNs) {
        if (buffer == null) {
            throw new IllegalArgumentException("buffer not allowed to be null");
        }
        if (rotation % 90 != 0) {
            throw new IllegalArgumentException("rotation must be a multiple of 90");
        }
        this.buffer = buffer;
        this.rotation = rotation;
        this.timestampNs = timestampNs;
    }

    public Buffer getBuffer() {
        return buffer;
    }

    /**
     * Rotation of the frame in degrees.
     */
    public int getRotation() {
        return rotation;
    }

    /**
     * Timestamp of the frame in nano seconds.
     */
    public long getTimestampNs() {
        return timestampNs;
    }

    public int getRotatedWidth() {
        if (rotation % 180 == 0) {
            return buffer.getWidth();
        }
        return buffer.getHeight();
    }

    public int getRotatedHeight() {
        if (rotation % 180 == 0) {
            return buffer.getHeight();
        }
        return buffer.getWidth();
    }

    public void retain() {
        buffer.retain();
    }

    public void release() {
        buffer.release();
    }
}
