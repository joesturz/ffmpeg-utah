all:	bouncer

bouncer:	bouncer.cc
	g++ -I ../include/ -L ../lib -o bouncer bouncer.cc `pkg-config --cflags --libs libavutil libavformat libavcodec libswscale`

clean:
	rm -f *.o bouncer *.utah *.mp4

movie:
	ffmpeg -f image2 -i frame%03d.utah movie.mp4
