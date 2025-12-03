#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "file.h"
#include "common.h"


int create_db_file(char *filename) {
	return open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
}

int open_db_file(char *filename) {
	return open(filename, O_RDWR);
}


