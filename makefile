all: clean inotify

inotify:
	cc -o inotify inotify.c -Wall -g

clean:
	-rm inotify
