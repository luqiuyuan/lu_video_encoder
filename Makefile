LIBS = \
	-lavcodec \
	-lavutil \
	-lopencv_core \
	-lopencv_highgui \

all:	test

test:	test.o video_encoder.o YUV_RGB_conversion.o
		g++ test.o video_encoder.o YUV_RGB_conversion.o -o test $(LIBS)

test.o:	test.cpp
		g++ -c test.cpp

video_encoder.o:	video_encoder.cpp
					g++ -c video_encoder.cpp

YUV_RGB_conversion.o:	YUV_RGB_conversion.cpp
						g++ -c YUV_RGB_conversion.cpp

clean:
	rm -f test.o video_encoder.o YUV_RGB_conversion.o