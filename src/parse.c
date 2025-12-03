#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

int create_db_header(struct dbheader_t **headerOut) {
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		return STATUS_ERROR;
	}

	header->version = 1;
	header->count = 0;
	header->magic = HEADER_MAGIC;
	header->filesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * header->count);

	*headerOut = header;
	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **headerOut) {
	if (fd < 0) {
		return STATUS_ERROR;
	}

	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) {
		return STATUS_ERROR;
	}

	if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		free(header);
		return STATUS_ERROR;
	}

	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->magic = ntohl(header->magic);
	header->filesize = ntohl(header->filesize);

	if (header->magic != HEADER_MAGIC) {
		free(header);
		return STATUS_ERROR;
	}

	if (header->version != 1) {
		free(header);
		return STATUS_ERROR;
	}

	struct stat dbstat = {0};
	fstat(fd, &dbstat);
	if (header->filesize != dbstat.st_size) {
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;
	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (fd < 0) {
		return STATUS_ERROR;
	}

	int realcount = dbhdr->count;
	
	struct dbheader_t temp_header = *dbhdr;
	temp_header.magic = htonl(temp_header.magic);
	temp_header.filesize = htonl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * realcount));
	temp_header.count = htons(temp_header.count);
	temp_header.version = htons(temp_header.version);

	lseek(fd, 0, SEEK_SET);
	write(fd, &temp_header, sizeof(struct dbheader_t));

	int i = 0;
	for (; i < realcount; i++) {
		struct employee_t temp_emp = employees[i];
		temp_emp.hours = htonl(temp_emp.hours);
		write(fd, &temp_emp, sizeof(struct employee_t));
	}

	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) {
	return STATUS_SUCCESS;
}

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t *employees, char *addstring) {
	return STATUS_SUCCESS;
}