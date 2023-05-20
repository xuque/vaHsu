package com.vaHsu.encoder;

import com.vaHsu.encoder.hard.AvcHardEncoder;

public class VideoEncoderFactory {
    public static IVideoEncoder createVideoEncoder(VideoCodecType videoEncoderType, boolean bSoft) {
        IVideoEncoder encoder = null;
        if (videoEncoderType == VideoCodecType.H264) {
            if (bSoft) {

            } else {
                encoder = new AvcHardEncoder();
            }
        } else if (videoEncoderType == VideoCodecType.H265) {
            if (bSoft) {

            } else {

            }
        }
        return encoder;
    }
}
