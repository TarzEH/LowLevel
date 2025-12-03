#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int create_db_header(struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (!header) return STATUS_ERROR;
	
	header->magic = HEADER_MAGIC;
	header->version = 1;
	header->count = 0;
	header->filesize = sizeof(struct dbheader_t);
	
	*headerOut = header;
	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	*headerOut = NULL;
	
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (!header) return STATUS_ERROR;
	
	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		free(header);
		return STATUS_ERROR;
	}
	
	header->magic = ntohl(header->magic);
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->filesize = ntohl(header->filesize);
	
	if (header->magic != HEADER_MAGIC || header->version != 1) {
		free(header);
		return STATUS_ERROR;
	}
	
	*headerOut = header;
	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	dbhdr->filesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * dbhdr->count);
	
	struct dbheader_t header = *dbhdr;
	header.magic = htonl(header.magic);
	header.version = htons(header.version);
	header.count = htons(header.count);
	header.filesize = htonl(header.filesize);
	
	lseek(fd, 0, SEEK_SET);
	write(fd, &header, sizeof(struct dbheader_t));
	
	int i;
	for (i = 0; i < dbhdr->count; i++) {
		write(fd, &employees[i], sizeof(struct employee_t));
	}
	
	return STATUS_SUCCESS;
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	return STATUS_SUCCESS;
}