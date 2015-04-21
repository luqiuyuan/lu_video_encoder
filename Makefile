LIBS = \
	-lavcodec \
	-lavutil \
	-lopencv_core \
	-lopencv_highgui \

all:	test

test:	test.o video_encoder.o
		g++ test.o video_encoder.o -o test $(LIBS)

test.o:	test.cpp
		g++ -c test.cpp

video_encoder.o:	video_encoder.cpp
					g++ -c video_encoder.cpp

clean:
	rm -f test.o video_encoder.o