#include <sys/types.h>	// used data types are here
#include <sys/stat.h>	// stat structure
#include <time.h>	// ctime()
#include <stdio.h>	// standart library
#include <stdlib.h>	// malloc()
#include <dirent.h>	// dirent structure
#include <pwd.h>	// getpwuid()
#include <grp.h>	// getgrgid
#include <string.h>	// strcmp(), strcpy()
#include <limits.h>	// NAME_MAX

int print_stats(char *filename, int show_hidden, struct stat *statbuf, int show_stats)	// ls -l, ls -la/-al
{
	struct passwd *userinfo;	// username
	struct group *groupinfo;	// groupname 

	if (show_stats && (show_hidden || ((filename[0] != '.') && !show_hidden))) {	// need to use the list format when -l or -la
		char *mode = malloc(sizeof(char) * (10 + 1));

		if (mode == NULL) {
			fprintf(stderr, "malloc() error!\n");
			return -1;
		}

		mode_t permissions = statbuf->st_mode;
			
		switch (permissions & S_IFMT) {
		case S_IFREG:
			mode[0] = '-';	// regular file
			break;	
		case S_IFBLK:
			mode[0] = 'b';	// block device    
			break;
		case S_IFCHR:
			mode[0] = 'c';	// character device
			break;
		case S_IFDIR:
			mode[0] = 'd';	// directory
			break;
		case S_IFLNK:
			mode[0] = 'l';	// symlink
			break;
		case S_IFIFO:
			mode[0] = 'p';	// FIFO/pipe
			break;
		case S_IFSOCK:
			mode[0] = 's';	// socket
			break;
		default:
			mode[0] = '?';	// ?
			break;
		} 

		mode[1] = (permissions & S_IRUSR) ? 'r' : '-';	// USR
		mode[2] = (permissions & S_IWUSR) ? 'w' : '-';

		if (permissions & S_ISUID) {
			mode[3] = 's';	// set-user-id bit	
		} else if (permissions & S_IXUSR) {
			mode[3] = 'x';
		} else {
			mode[3] = '-';
		}

		mode[4] = (permissions & S_IRGRP) ? 'r' : '-';	// GRP
		mode[5] = (permissions & S_IWGRP) ? 'w' : '-';

		if (permissions & S_ISGID) {
			mode[6] = 's';	// set-group-id bit		
		} else if (permissions & S_IXGRP) {
			mode[6] = 'x';
		} else {
			mode[6] = '-';
		}

		mode[7] = (permissions & S_IROTH) ? 'r' : '-';	// OTH
		mode[8] = (permissions & S_IWOTH) ? 'w' : '-';

		
		if (permissions & S_ISVTX) {
			mode[9] = 't';	// sticky-bit			
		} else if (permissions & S_IXOTH) {
			mode[9] = 'x';
		} else {
			mode[9] = '-';
		}

		mode[10] = '\0'; 

		userinfo = getpwuid(statbuf->st_uid);
			
		if (userinfo == NULL) {
			fprintf(stderr, "getpwuid() error!\n");
			return -1;
		}

		groupinfo = getgrgid(statbuf->st_gid);

		if (groupinfo == NULL) {
			fprintf(stderr, "getgrgid() error!\n");
			return -1;
		}

		char *mod_time = (char *) malloc(sizeof(char) * (25+1));	// Wed Jan 02 02:03:55 1980\n\0 => 26

		if (mod_time == NULL) {
			fprintf(stderr, "malloc() error!\n");
			return -1;
		}

		strcpy(mod_time, ctime(&statbuf->st_mtime));
		mod_time[24] = ' ';	// to delete '\n'

		printf("\n%s\t%ld\t%s\t%s\t%lld\t%s\t", 
			mode, (long) statbuf->st_nlink, userinfo->pw_name, groupinfo->gr_name, 
			(long long) statbuf->st_size, mod_time);

		free(mode);
		free(mod_time);
	}

	if (show_hidden || ((filename[0] != '.') && !show_hidden)) {
		printf("%s\t", filename);
	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc > 3) {
		fprintf(stderr, "Usage:\t%s <flags: -l, -a, -la, -al> <path>\n", argv[0]);
		return -1;
	}
	
	int show_all = 0, show_list = 0, show_la = 0;	// -a, -l, -la/-al options flags
		
	if (argc > 1) {
		show_all = (!(strcmp(argv[1], "-a"))) ? 1 : 0;
		show_list = (!(strcmp(argv[1], "-l"))) ? 1 : 0;
		show_la = (!(strcmp(argv[1], "-la")) || !(strcmp(argv[1], "-al")));
	}

	int opt_enabled = (show_all || show_list || show_la);	// at least one option, argv[1]
	int path_index = (opt_enabled) ? 2 : 1;	// argv[2] or argv[1], path is in first or second _passed_ argument
	int default_path = ((argc == 1) || ((argc == 2) && opt_enabled));	// checking for empty <path> in arguments

	const char *current_dir = ".";	// current dir path as default for ls command
	long int path_length = (default_path) ? strlen(current_dir) : strlen(argv[path_index]);

	DIR *dir;
	struct dirent *current_file;
	struct stat sb;	// type, permissions, size, links, user ID, group ID, size, time of last modification

	dir = (default_path) ? (opendir(current_dir)) : (opendir(argv[path_index]));	// open directory
	int not_dir = (dir == NULL) ? 1 : 0;	// it's a file? - the path in argv[1] or argv[2] isn't a directory, maybe it's a file?

	if (not_dir) {
		if (stat(argv[path_index], &sb) == -1) {
			fprintf(stderr, "No such file or directory!\n");	// isn't a directory, isn't a file
			return -1;
		} else {
			if (print_stats(argv[path_index], (show_all || show_la), &sb, (show_list || show_la)) != 0) {
				fprintf(stderr, "print_stats error!\n");
				return -1;
			}
			printf("\n");
		}
	}

	while (!not_dir && ((current_file = readdir(dir)) != NULL)) {
		char *readdir_file_path = (char *) malloc(sizeof(char) * (path_length + NAME_MAX + 2));

		if (readdir_file_path == NULL) {
			fprintf(stderr, "malloc() error!\n");
			return -1;
		}
		
		if (default_path) {
			sprintf(readdir_file_path, "%s/%s", current_dir, current_file->d_name);
		} else {
			sprintf(readdir_file_path, "%s/%s", argv[path_index], current_file->d_name);
		} 

		if (stat(readdir_file_path, &sb) == -1) {
			fprintf(stderr, "stat() error!\n");
			return -1;
		}

		if (print_stats(current_file->d_name, (show_all || show_la), &sb, (show_list || show_la)) != 0) {
			fprintf(stderr, "print_stats error!\n");
			return -1;
		}

		free(readdir_file_path);
	}

	if (!not_dir) {
		printf("\n");
		closedir(dir);	// close directory
	}

	return 0;
}

