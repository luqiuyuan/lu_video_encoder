#ifndef __LU_VIDEO_ENCODER__
#define __LU_VIDEO_ENCODER__

#include "YUV_RGB_conversion.h"

#include <string>

#include <opencv2/opencv.hpp>

// ffmpeg headers
extern "C"{
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
}

using namespace std;
using namespace cv;

class VideoEncoder {

private:

	int video_codec;

	Size resolution;

	FILE* output;

	AVCodecContext* c;

	AVFrame* av_frame;

	AVPacket pkt;

	int initialization_flag;

	static uint8_t ENDCODE[];

	// default bitrate in bit (1M/s)
	static const int DEFAULT_BITRATE = 1000000;

	// default fps
	static const int DEFAULT_FPS = 25;

	// default size of GOP
	static const int DEFAULT_SIZE_GOP = 300;

	// default max B frames between I or P frames
	static const int DEFAULT_B_FRAME = 3;

	// default H.264 preset (medium)
	static string DEFAULT_H264_PRESET;

	int frameN;	// presentation timestamp. Default -1. Use it as ++ frameN.

	int got_output;	// flag of avcodec_encode_video2, no need to initialize.

	/**
	 * flush the buffered frames to the file and close the codec
	 * 
	 * @param non
	 * @return true if flush is successful, false otherwise
	 */
	bool flush();

	/**
	 * convert BGR Mat to YUV mat
	 * Note that the imread returns BGR Mat in default in OpenCV
	 * 
	 * @param input BGR Mat of type CV_8UC3
	 * @return YUV Mat of type CV_8UC3. It shares different memory from the input Mat.
	 */
	Mat BGR2YUV(const Mat& input);

public:

	static const int VIDEO_CODEC_H264 = 1;

	VideoEncoder(string filepath, Size resolution, int video_codec);

	~VideoEncoder();

	/**
	 * encode a frame of BGR Mat
	 * 
	 * @param input BGR Mat of type CV_8UC3
	 * @return true if encoding is successful, false otherwise
	 */
	bool write(const Mat& frame);

	// initialization status flag (==0 success, <0 error, >0 warning)
	//  0 - successfull
	// -1 - could not open file
	// -2 - could not
	// -3 - could not allocate video codec context
	// -4 - could not open codec
	// -5 - could not allocate frame
	// -6 - could not allocate raw picture buffer
	//  1 - Undefined video codec. Use default H.264 codec
	int getInitializationFlag();

	/**
	 * flush the buffered frames to the file and close the codec and the output file
	 * 
	 * @param non
	 * @return true if flush is successful, false otherwise
	 */
	bool flush_close();

};

#endif