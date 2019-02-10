CC = gcc
CFLAGS = -g -Wall -std=c11 -D_XOPEN_SOURCE=700

RM = rm -rf

all: clean inotify

inotify:
	$(CC) -o inotify inotify.c $(CFLAGS)

clean:
	-$(RM) inotify
