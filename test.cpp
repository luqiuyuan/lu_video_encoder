#include "video_encoder.h"

#include <iostream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main() {
	VideoCapture cap("./input/office.mp4");
	if(!cap.isOpened()){
		cout<<"cannot open file."<<endl;
		return 1;
	}

	int width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	VideoEncoder encoder("./output/office.h264", Size(width, height), VideoEncoder::VIDEO_CODEC_H264);
	int initialization_flag = encoder.getInitializationFlag();
	if(initialization_flag < 0) {
		cerr<<"Error: main.cpp: Video encoder initialization failed with error code: "<<initialization_flag<<endl;
		return 1;
	} else if(initialization_flag > 0)
		cerr<<"Warning: main.cpp: Video encoder initialization warning with warning code: "<<initialization_flag<<endl;

	while(1){
		Mat frame;

		bool bSuccess = cap.read(frame);

		if(!bSuccess){
			cout<<"cannot read frame."<<endl;
			break;
		}

		encoder.write(frame);
	}

	encoder.flush_close();

	return 0;
}