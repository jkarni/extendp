xp : extend.c
	gcc -O2 -Wall -lreadline extend.c `pkg-config --cflags --libs glib-2.0` -o xp


