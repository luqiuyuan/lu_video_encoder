#include "YUV_RGB_conversion.h"

/**
 * Clip a value to [0,255]
 * @param v
 * @return A value within [0,255]
 */
unsigned char clip(int v);

vector<unsigned char> YUV2RGB(vector<unsigned char> yuv) {
	unsigned char y = yuv[0];
	unsigned char u = yuv[1];
	unsigned char v = yuv[2];

	// the intermediate result
	int c = y - 16;
	int d = u - 128;
	int e = v - 128;

	unsigned char r;
	unsigned char g;
	unsigned char b;
	r = clip((298*c + 409*e + 128) >> 8);
	g = clip((298*c - 100*d - 208*e + 128) >> 8);
	b = clip((298*c + 516*d + 128) >> 8);

	// construct the return value
	vector<unsigned char> rgb(3);
	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;

	return rgb;
}

vector<unsigned char> RGB2YUV(vector<unsigned char> rgb) {
	unsigned char r = rgb[0];
	unsigned char g = rgb[1];
	unsigned char b = rgb[2];

	unsigned char y;
	unsigned char u;
	unsigned char v;
	y = ((66*r + 129*g +25*b + 128) >> 8) + 16;
	u = ((-38*r - 74*g + 112*b + 128) >> 8) + 128;
	v = ((112*r - 94*g - 18*b + 128) >> 8) + 128;

	// construct the return value
	vector<unsigned char> yuv(3);
	yuv[0] = y;
	yuv[1] = u;
	yuv[2] = v;

	return yuv;
}

unsigned char clip(int v) {
	if(v < 0)
		return 0;
	if(v > 255)
		return 255;
	return v;
}