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
	header->filesize = ntohl(sizeof(struct dbheader_t) + (sizeof(struct employee_t) * header->count));

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
	temp_header.filesize = htonl(temp_header.filesize);
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

int list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) {
	if (!dbhdr || !employees) {
		return STATUS_ERROR;
	}

	int i;
	for (i = 0; i < dbhdr->count; i++) {
		printf("Employee %d:\n", i + 1);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %u\n", employees[i].hours);
	}

	return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) {
	if (!dbhdr || !employees || !addstring) {
		return STATUS_ERROR;
	}

	char *name = strtok(addstring, ",");
	char *address = strtok(NULL, ",");
	char *hours_str = strtok(NULL, ",");

	if (!name || !address || !hours_str) {
		return STATUS_ERROR;
	}

	unsigned int hours = atoi(hours_str);

	*employees = realloc(*employees, (dbhdr->count + 1) * sizeof(struct employee_t));
	if (!*employees) {
		return STATUS_ERROR;
	}

	strncpy((*employees)[dbhdr->count].name, name, NAME_LEN - 1);
	(*employees)[dbhdr->count].name[NAME_LEN - 1] = '\0';
	strncpy((*employees)[dbhdr->count].address, address, ADDRESS_LEN - 1);
	(*employees)[dbhdr->count].address[ADDRESS_LEN - 1] = '\0';
	(*employees)[dbhdr->count].hours = hours;

	dbhdr->count++;
	dbhdr->filesize = sizeof(struct dbheader_t) + (sizeof(struct employee_t) * dbhdr->count);

	return STATUS_SUCCESS;
}