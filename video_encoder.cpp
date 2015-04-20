#include "video_encoder.h"

uint8_t VideoEncoder::ENDCODE[] = {0, 0, 1, 0xb7};

string VideoEncoder::DEFAULT_H264_PRESET = "medium";

VideoEncoder::VideoEncoder(string filepath, Size resolution, int video_codec): resolution(resolution), video_codec(video_codec), c(NULL), frameN(-1) {
	// temperal variables declaration
	int result;

	output = fopen(filepath.c_str(), "wb");
	if(!output) {
    	cerr<<"Error: video_encoder.cpp: Could not open "<<filepath<<endl;
    	initialization_flag = -1;
    	return;
    }

    /* register all the codecs */
    avcodec_register_all();

    AVCodecID video_codec_id;
    if(video_codec == VIDEO_CODEC_H264)
    	video_codec_id = AV_CODEC_ID_H264;
    else {	// invalid codec option. use default video codec
    	cerr<<"Warning: video_encoder.cpp: Invalid video codec option: "<<video_codec<<". Default video codec H.264 is used."<<endl;
    	video_codec_id = AV_CODEC_ID_H264;
    	initialization_flag = 1;
    }

    AVCodec* codec;
    codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        cerr<<"Error: VideoEncoder::VideoEncoder: codec not found."<<endl;
        initialization_flag = -2;
        return;
    }

    c = avcodec_alloc_context3(codec);
    if(!c) {
    	cerr<<"Error: video_encoder.cpp: Could not allocate video codec context"<<endl;
    	initialization_flag = -3;
    	return;
    }

    /* put sample parameters */
    c->bit_rate = DEFAULT_BITRATE;
    /* resolution must be a multiple of two */
    c->width = resolution.width;
    c->height = resolution.height;
    /* frames per second */
    c->time_base = (AVRational){1,DEFAULT_FPS};
    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    c->gop_size = DEFAULT_SIZE_GOP;
    c->max_b_frames = DEFAULT_B_FRAME;
    c->pix_fmt = AV_PIX_FMT_YUV444P;	// use AV_PIX_FMT_YUV444P to facilitate data transfer from cv::Mat

    if (video_codec_id == AV_CODEC_ID_H264){
       av_opt_set(c->priv_data, "preset", DEFAULT_H264_PRESET.c_str(), 0);	// use H.264 preset
       string qp = "0";
       av_opt_set(c->priv_data, "qp", qp.c_str(), 0);
       string verbose = "40";
       av_opt_set(c->priv_data, "verbose", verbose.c_str(), 0);
    }

    /* open codec */
    if (avcodec_open2(c, codec, NULL) < 0) {
    	cerr<<"Error: video_encoder.cpp: Could not open codec"<<endl;
    	initialization_flag = -4;
    	return;
 	}

 	// allocate frame
 	av_frame = av_frame_alloc();
    if(!av_frame) {
    	cerr<<"Error: video_encoder.cpp: Could not allocate video frame"<<endl;
    	initialization_flag = -5;
    	return;
    }
    av_frame->format = c->pix_fmt;
    av_frame->width = c->width;
    av_frame->height = c->height;

    // allocate raw picture buffer
    result = av_image_alloc(av_frame->data, av_frame->linesize, c->width, c->height, c->pix_fmt, 32);
    if(result < 0) {
    	cerr<<"Error: video_encoder: Could not allocate raw picture buffer"<<endl;
    	initialization_flag  = -6;
    	return;
    }
}

VideoEncoder::~VideoEncoder() {
	fclose(output);
}

bool VideoEncoder::write(const Mat& frame) {
    // convert frame to YUV format
    Mat frame_yuv = BGR2YUV(frame);
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    // size of the input frame
    Size size = frame_yuv.size();
    if(size.width != c->width || size.height != c->height){
        cerr<<"Error: video_encoder.cpp: The resolution of frame changed."<<endl;
        return false;
    }

    /* prepare image */
    vector<Mat> mv;
    split(frame_yuv, mv);    // split channels to facilitate the memory copy
    if(mv[0].isContinuous())    // if the matrix is in a single row in memory
        memcpy(av_frame->data[0], mv[0].data, c->height*c->width);
    else    // if there are gaps between rows
        for(int row = 0; row < c->height; row++) {
            memcpy(&(av_frame->data[0][row*c->width]), mv[0].ptr<uchar>(row), c->width);
        }
    if(mv[1].isContinuous())    // if the matrix is in a single row in memory
        memcpy(av_frame->data[1], mv[1].data, c->height*c->width);
    else    // if there are gaps between rows
        for(int row = 0; row < c->height; row++) {
            memcpy(&(av_frame->data[1][row*c->width]), mv[1].ptr<uchar>(row), c->width);
        }
    if(mv[2].isContinuous())    // if the matrix is in a single row in memory
        memcpy(av_frame->data[2], mv[2].data, c->height*c->width);
    else    // if there are gaps between rows
        for(int row = 0; row < c->height; row++) {
            memcpy(&(av_frame->data[2][row*c->width]), mv[2].ptr<uchar>(row), c->width);
        }
    
    // presentation timestamp. starts from 0.
    av_frame->pts = ++frameN;

    /* encode the image */
    int result;
    result = avcodec_encode_video2(c, &pkt, av_frame, &got_output);
    if (result < 0) {
        cerr<<"Error: video_encoder.cpp: Error in encoding frame #"<<frameN<<"."<<endl;
        return false;
    }

    if (got_output) {
        fwrite(pkt.data, 1, pkt.size, output);
        av_free_packet(&pkt);
    }

	return true;
}

bool VideoEncoder::flush() {
    /* flush the delayed frames */
    int result;
    while(got_output) {
        result = avcodec_encode_video2(c, &pkt, NULL, &got_output);
        if (result < 0) {
            cerr<<"Error: video_encoder.cpp: Error in encoding frame #"<<frameN<<"."<<endl;
            return false;
        }

        if (got_output) {
            fwrite(pkt.data, 1, pkt.size, output);
            av_free_packet(&pkt);
        }
    }

    /* add sequence end code to have a real mpeg file */
    fwrite(ENDCODE, 1, sizeof(ENDCODE), output);

    avcodec_close(c);
    av_free(c);
    av_freep(&av_frame->data[0]);
    av_frame_free(&av_frame);

    return true;
}

bool VideoEncoder::flush_close() {
    bool result = flush();
    fclose(output); // close the file no matter flush is successful or not.
    return result;
}

int VideoEncoder::getInitializationFlag() {
	return initialization_flag;
}

Mat VideoEncoder::BGR2YUV(const Mat& input){
    Size inputSize = input.size();

    Mat output(inputSize, CV_8UC3);

    // split input Mat
    // the input is BGR Mat, because imread() returns a BGR Mat in OpenCV
    // inputChannels[0] is blue channel
    // inputChannels[1] is green channel
    // inputChannels[2] is red channel
    vector<Mat> inputChannels;
    split(input, inputChannels);

    // calculate YUV Mat
    // conversion formulae can be found in https://msdn.microsoft.com/en-ca/library/aa917087.aspx
    for(int i = 0; i < inputSize.height; i++){
        for(int j = 0; j < inputSize.width; j++){
            // prepare the input parameter of RGB2YUV
            vector<uchar> rgb(3);
            rgb[0] = inputChannels[2].at<uchar>(i,j);   // note that OpenCV convention is BGR, instead of RGB
            rgb[1] = inputChannels[1].at<uchar>(i,j);
            rgb[2] = inputChannels[0].at<uchar>(i,j);   // note that OpenCV convention is BGR, instead of RGB

            vector<uchar> yuv = RGB2YUV(rgb);
            uchar y = yuv[0];
            uchar u = yuv[1];
            uchar v = yuv[2];

            output.at<Vec3b>(i,j)[0] = y;
            output.at<Vec3b>(i,j)[1] = u;
            output.at<Vec3b>(i,j)[2] = v;
        }
    }

    return output;
}