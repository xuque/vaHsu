package com.vaHsu.encoder;

/** Enumeration of supported video codec types. */
enum VideoCodecType {
    VP8("video/x-vnd.on2.vp8"),
    VP9("video/x-vnd.on2.vp9"),
    H264("video/avc"),
    H265("video/hevc"),
    AV1("video/av01");

    private final String mimeType;

    private VideoCodecType(String mimeType) {
        this.mimeType = mimeType;
    }

    String mimeType() {
        return mimeType;
    }
}
