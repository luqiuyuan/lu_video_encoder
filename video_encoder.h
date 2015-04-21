#ifndef __LU_VIDEO_ENCODER__
#define __LU_VIDEO_ENCODER__

#include <string>
#include <vector>

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

	// default bitrate in bit (8Mbits/s)
	static const int DEFAULT_BITRATE = 8000000;

	// bitrate in bit
	int bitrate;

	// default quantization parameter (30)
	static string DEFAULT_H264_QP;

	// quantization parameter
	string h264_qp;

	// default fps
	static const int DEFAULT_FPS = 25;

	// fps
	int fps;

	// default size of GOP
	static const int DEFAULT_SIZE_GOP = 300;

	// size of GOP
	int size_gop;

	// default max B frames between I or P frames
	static const int DEFAULT_B_FRAME = 3;

	// max B frames between I or P frames
	int b_frame;

	// default H.264 preset (medium)
	static string DEFAULT_H264_PRESET;

	// h264 preset
	string h264_preset;

	// default H.264 profile
	static string DEFAULT_H264_PROFILE;

	// H.264 profile
	string h264_profile;

	int frameN;	// presentation timestamp. Default -1. Use it as ++ frameN.

	int got_output;	// flag of avcodec_encode_video2, no need to initialize.

	bool disable_bitrate_control;

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

	/**
	 * Convert RGB to YUV
	 * @param rgb A vector of 3 containing R, G and B components. The range is [0,255]
	 * @return A vector of 3 containing Y, U and V components. The first element is Y. The range is [0,255]
	 */
	vector<unsigned char> RGB2YUV(vector<unsigned char> rgb);

public:

	static const int VIDEO_CODEC_H264 = 1;

	VideoEncoder(string filepath, Size resolution, int video_codec);

	~VideoEncoder();

	/**
	 * initializing codec. Seperate it from the constructor to enable user-controled parameters
	 * 
	 * @param none
	 * @return none
	 */
	void init();

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

	/**
	 * set quantization parameter
	 * 
	 * @param qp 0-69. 0 is lossless.
	 * @return none
	 */
	void setQP(int qp);

	/**
	 * set bitrate
	 * 
	 * @param bitrate in bits/s
	 * @return none
	 */
	void setBitrate(int bitrate);

	/**
	 * set fps
	 * 
	 * @param fps
	 * @return none
	 */
	void setFPS(int fps);

	/**
	 * set size of GOP
	 * 
	 * @param size_gop
	 * @return none
	 */
	void setSizeGOP(int size_gop);

	/**
	 * set max B frames between I or P frames
	 * 
	 * @param b_frame
	 * @return none
	 */
	void setBFrame(int b_frame);

	/**
	 * set h.264 preset
	 * 
	 * @param preset string
	 * @return none
	 */
	void setPreset(string preset);

	/**
	 * set h.264 profile
	 * 
	 * @param profile string
	 * @return none
	 */
	void setProfile(string profile);

	/**
	 * bit rate control is used by default. Calling disableBitrateControl switch to fixed quantization parameter
	 * 
	 * @param none
	 * @return none
	 */
	void disableBitrateControl();

};

#endif