#ifndef __YUV_RGB_CONVERSION__
#define __YUV_RGB_CONVERSION__

/**
	This file contains the functions to do YUV and RGB color space conversion. The conversion formulae come from https://msdn.microsoft.com/en-us/library/ms893078.aspx
	@author Lu Sun
	@version 0.0 03/03/2015
 */

#include <vector>

using namespace std;

/**
 * Convert YUV to RGB
 * @param yuv A vector of 3 containing Y, U and V components. The range is [0,255]
 * @return A vector of 3 containing R, G and B components. The first element is R. The range is [0,255]
 */
vector<unsigned char> YUV2RGB(vector<unsigned char> yuv);

/**
 * Convert RGB to YUV
 * @param rgb A vector of 3 containing R, G and B components. The range is [0,255]
 * @return A vector of 3 containing Y, U and V components. The first element is Y. The range is [0,255]
 */
vector<unsigned char> RGB2YUV(vector<unsigned char> rgb);

#endif