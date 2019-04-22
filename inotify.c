/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 *
 * inotify(7) full sample
 *
 * MIT License
 *
 */

/* inotify.c
 * inotify(7) full sample
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 22/01/2019 | Creation
 *          |            |
 */


/* *** INCLUDES ************************************************************************ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h> /* PATH_MAX */
#include <sys/inotify.h>


/* *** DEFINES AND LOCAL DATA TYPE DEFINATION ****************************************** */
#define OK				(0)
#define NOK				(-1)
#define MAX_ENTRY		(5)
#define MAX_FLAG_NAME		(100)

typedef struct _entry_t{
	char path[PATH_MAX + 1];
	uint32_t flags;
}entry_t;

/* *** LOCAL PROTOTYPES (if applicable) ************************************************ */


/* *** EXTERNS / LOCAL / GLOBALS VARIEBLES ********************************************* */


/* *** FUNCTIONS *********************************************************************** */
/* <header function description ONLY for local functions (not in .h)> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
void usage(int argc, char *argv[])
{
	printf("Usage:\n\t%s 'path_to_file_to_watch_1:FLAG1|FLAG2|FLAG3' 'path_to_file_to_watch_2:FLAG1' '...' '...' '...'\n\n(max %d paths just for convenience)\n\n", argv[0], MAX_ENTRY);
	printf("inotify_add_watch() flags (see inofity(7)):\n");
	printf("\tIN_ACCESS (+) ......: File was accessed (e.g., read(2), execve(2))\n");
	printf("\tIN_MODIFY (+) ......: File was modified\n");
	printf("\tIN_ATTRIB (*) ......: Metadata changed (e.g., chmod(2)), timestamps (e.g., utimensat(2)), extended attributes (setxattr(2)),\n" \
	       "\t                      link count (since Linux 2.6.25; e.g., for the target of link(2) and for unlink(2)), and user/group ID (chown(2))\n");
	printf("\tIN_CLOSE_WRITE (+) .: Writtable file was closed\n");
	printf("\tIN_CLOSE_NOWRITE (*): Unwrittable file closed\n");
	printf("\tIN_CLOSE ...........: (IN_CLOSE_WRITE | IN_CLOSE_NOWRITE) Close\n");
	printf("\tIN_OPEN (*) ........: File was opened\n");
	printf("\tIN_MOVED_FROM (+) ..: File was moved from X\n");
	printf("\tIN_MOVED_TO (+) ....: File was moved to Y\n");
	printf("\tIN_MOVE ............: (IN_MOVED_FROM | IN_MOVED_TO) Moves\n");
	printf("\tIN_CREATE (+) ......: Subfile was created\n");
	printf("\tIN_DELETE (+) ......: Subfile was deleted\n");
	printf("\tIN_DELETE_SELF .....: Self was deleted\n");
	printf("\tIN_MOVE_SELF .......: Self was moved\n\n");

	printf("Events sent by the kernel\n");
	printf("\tIN_UNMOUNT .........: Backing fs was unmounted\n");
	printf("\tIN_Q_OVERFLOW ......: Event queued overflowed\n");
	printf("\tIN_IGNORED .........: File was ignored\n\n");

	printf("Special flags\n");
	printf("\tIN_ONLYDIR .........: Only watch the path if it is a directory\n");
	printf("\tIN_DONT_FOLLOW .....: Do not follow a sym link\n");
	printf("\tIN_EXCL_UNLINK .....: Exclude events on unlinked objects\n");
	printf("\tIN_MASK_ADD ........: Add to the mask of an already existing watch\n");
	printf("\tIN_ISDIR ...........: Event occurred against dir\n");
	printf("\tIN_ONESHOT .........: Only send event once, then remove from watch list\n\n");

	printf("All events which a program can wait on:\n");
	printf("\tIN_ALL_EVENTS...: (IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_OPEN | IN_MOVED_FROM |\n" \
	       "\t                   IN_MOVED_TO | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE_SELF)\n\n");

	printf("(the events marked above with an asterisk (*) can occur both for the directory itself and for objects inside the directory; and\n" \
	       " the events marked with a plus sign (+) occur only for objects inside the directory (not for the directory itself))\n\n");

	return;
}


/* <header function description ONLY for local functions (not in .h)> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
uint32_t paramToFlag(char *s)
{
	if     (strcmp(s, "IN_ACCESS")        == 0) return(IN_ACCESS);
	else if(strcmp(s, "IN_MODIFY")        == 0) return(IN_MODIFY);
	else if(strcmp(s, "IN_ATTRIB")        == 0) return(IN_ATTRIB);
	else if(strcmp(s, "IN_CLOSE_WRITE")   == 0) return(IN_CLOSE_WRITE);
	else if(strcmp(s, "IN_CLOSE_NOWRITE") == 0) return(IN_CLOSE_NOWRITE);
	else if(strcmp(s, "IN_CLOSE")         == 0) return(IN_CLOSE);
	else if(strcmp(s, "IN_OPEN")          == 0) return(IN_OPEN);
	else if(strcmp(s, "IN_MOVED_FROM")    == 0) return(IN_MOVED_FROM);
	else if(strcmp(s, "IN_MOVED_TO")      == 0) return(IN_MOVED_TO);
	else if(strcmp(s, "IN_MOVE")          == 0) return(IN_MOVE);
	else if(strcmp(s, "IN_CREATE")        == 0) return(IN_CREATE);
	else if(strcmp(s, "IN_DELETE")        == 0) return(IN_DELETE);
	else if(strcmp(s, "IN_DELETE_SELF")   == 0) return(IN_DELETE_SELF);
	else if(strcmp(s, "IN_MOVE_SELF")     == 0) return(IN_MOVE_SELF);
	else if(strcmp(s, "IN_UNMOUNT")       == 0) return(IN_UNMOUNT);
	else if(strcmp(s, "IN_Q_OVERFLOW")    == 0) return(IN_Q_OVERFLOW);
	else if(strcmp(s, "IN_IGNORED")       == 0) return(IN_IGNORED);
	else if(strcmp(s, "IN_ONLYDIR")       == 0) return(IN_ONLYDIR);
	else if(strcmp(s, "IN_DONT_FOLLOW")   == 0) return(IN_DONT_FOLLOW);
	else if(strcmp(s, "IN_EXCL_UNLINK")   == 0) return(IN_EXCL_UNLINK);
	else if(strcmp(s, "IN_MASK_ADD")      == 0) return(IN_MASK_ADD);
	else if(strcmp(s, "IN_ISDIR")         == 0) return(IN_ISDIR);
	else if(strcmp(s, "IN_ONESHOT")       == 0) return(IN_ONESHOT);
	else if(strcmp(s, "IN_ALL_EVENTS")    == 0) return(IN_ALL_EVENTS);

	printf("Unknow flag: [%s]\n", s);

	return(0);
}


/* <header function description ONLY for local functions (not in .h)> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int fill(entry_t *paths, char *param)
{
	char *c1 = NULL, *c2 = NULL;
	char flag[MAX_FLAG_NAME] = {0};

	c1 = c2 = param;
	paths->flags = 0;

	c2 = strchr(c1, ':');
	if(c2 == NULL)
		return(NOK);

	strncpy(paths->path, c1, ((PATH_MAX < c2-c1) ? PATH_MAX : c2-c1));

	c1 = ++c2;

	for(;c1 != NULL; c1 = c2){

		c2 = strchr(c1, '|');
		if(c2 == NULL)
			strcpy(flag, c1);
		else{
			sprintf(flag, "%.*s", (int)((MAX_FLAG_NAME-1 < c2-c1) ? MAX_FLAG_NAME-1 : c2-c1), c1);
			c2++;
		}

		paths->flags |= paramToFlag(flag);
	}

	return(OK);
}


/* <header function description ONLY for local functions (not in .h)> void function(void)
 *
 * <Description>
 *
 * INPUT:
 *  <None>
 * OUTPUT:
 *  <None>
 */
int param(entry_t *paths, int argc, char *argv[])
{
	unsigned int i = 0;

	for(i = 0; i < argc-1; i++){

		if(fill(&paths[i], argv[i+1]) == NOK){
			printf("Erro parametro: [%s]\n", argv[i+1]);
			return(NOK);
		}
	}

	return(OK);
}


int main(int argc, char *argv[])
{
	entry_t paths[MAX_ENTRY];
	int fd = 0;
	char buf[sizeof(struct inotify_event) + PATH_MAX] = {'\0'};
	int wd = 0; /* watch descriptor */
	int len = 0;
	int i = 0;

	if(argc == 1 || argc > 6){
		usage(argc, argv);
		return(-1);
	}

	memset(&paths, 0, sizeof(sizeof(entry_t) * 5));

	if(param(paths, argc, argv) == NOK){
		return(-1);
	}

	fd = inotify_init();
	if (fd == -1){
		printf("inotify_init() error: [%s]\n", strerror(errno));
		return(-1);
	}


	for (i = 0; i < argc-1; i++) {
		wd = inotify_add_watch (fd, paths[i].path, paths[i].flags);

		if(wd == -1){
			printf("inotify_add_watch() error %d: [%s]\n",errno,  strerror(errno));
			return(-1);
		}
	}

	while(1){

		printf("Aguardando evento...\n");
		len = read(fd, buf, sizeof(buf));

		if(len == 0)
			printf("Evento recebido:\n");
		else if(len == 0)
			break;
		else if(len < 0){
			printf("read() error: [%s]\n", strerror(errno));
			return(-1);
		}

		i = 0;

		while (i < len) {
			struct inotify_event *ie = (struct inotify_event*) &buf[i];

			printf("\tEvento em [%s]: ", paths[ie->wd - 1].path);

			if(ie->mask & IN_ACCESS)        printf("evento IN_ACCESS [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_MODIFY)        printf("[%s] foi modificado. Evento IN_MODIFY.\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_ATTRIB)        printf("evento IN_ATTRIB [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_CREATE)        printf("[%s] foi criado. Evento IN_CREATE.\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_DELETE)        printf("[%s] foi deletado. Evento IN_DELETE.\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_CLOSE_WRITE)   printf("evento IN_CLOSE_WRITE [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_CLOSE_NOWRITE) printf("evento IN_CLOSE_NOWRITE [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_CLOSE)         printf("evento IN_CLOSE [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_OPEN)          printf("evento IN_OPEN [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_MOVED_FROM)    printf("evento IN_MOVED_FROM [%s] cookie: [%u].\n\n", ((ie->len != 0) ? ie->name : "no_name"), ie->cookie);
			if(ie->mask & IN_MOVED_TO)      printf("evento IN_MOVED_TO [%s] cookie: [%u].\n\n", ((ie->len != 0) ? ie->name : "no_name"), ie->cookie);
			if(ie->mask & IN_MOVE)          printf("evento IN_MOVE [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_DELETE_SELF)   printf("evento IN_DELETE_SELF [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_MOVE_SELF)     printf("evento IN_MOVE_SELF [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_UNMOUNT)       printf("evento IN_UNMOUNT [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_Q_OVERFLOW)    printf("evento IN_Q_OVERFLOW [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_IGNORED)       printf("evento IN_IGNORED [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_ONLYDIR)       printf("evento IN_ONLYDIR [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_DONT_FOLLOW)   printf("evento IN_DONT_FOLLOW [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_EXCL_UNLINK)   printf("evento IN_EXCL_UNLINK [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_MASK_ADD)      printf("evento IN_MASK_ADD [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_ISDIR)         printf("evento IN_ISDIR [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));
			if(ie->mask & IN_ONESHOT)       printf("evento IN_ONESHOT [%s].\n\n", ((ie->len != 0) ? ie->name : "no_name"));

			if(ie->mask & !(IN_ACCESS | IN_MODIFY | IN_ATTRIB | IN_CREATE | IN_DELETE |
			               IN_CLOSE_WRITE | IN_CLOSE_NOWRITE | IN_CLOSE | IN_OPEN |
			               IN_MOVED_FROM | IN_MOVED_TO | IN_MOVE | IN_DELETE_SELF |
			               IN_MOVE_SELF | IN_UNMOUNT | IN_Q_OVERFLOW | IN_IGNORED |
			               IN_ONLYDIR | IN_DONT_FOLLOW | IN_EXCL_UNLINK |
			               IN_MASK_ADD | IN_ISDIR | IN_ONESHOT))
				printf("Evento desconhecido!\n\n");

			i += sizeof(struct inotify_event) + ie->len;
		}
	}

	return(0);
}
