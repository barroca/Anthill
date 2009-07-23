/* By Sean Middleditch <elanthis@awemud.net>
   released into public domain, July 8th, 2003 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvm3.h"

#include "cser.h"

/* serialize a string - make it into one line */
int _cser_serialize(FILE* file, const char* value, size_t size) {
	size_t last, cur;

	for (cur = last = 0; cur < size; ++cur) {
		/* escape */
		if (value[cur] == '\\' || value[cur] == '\n' || value[cur] == '\0') {
			if (cur > last)
				fprintf(file, "%.*s", cur - last, &value[last]);
			fprintf(file, "\\%c", value[cur] == '\n' ? 'n' : value[cur] == '\0' ? '0' : value[cur]);
			last = cur + 1;
		}
	}

	/* pump out rest of buffer */
	if (cur > last)
		fprintf(file, "%.*s", cur - last, &value[last]);

	return 0;
}

/* write out a string */
int cser_write_str(FILE* file, const char* name, const char* value) {
	fprintf(file, "%s=", name);
	if (_cser_serialize(file, value, strlen(value)))
		return -1;
	fprintf(file, "\n");
	return 0;
}

/* write out a number */
int cser_write_num(FILE* file, const char* name, long num) {
	fprintf(file, "%s=%ld\n", name, num);
	return 0;
}

/* read in one line */
int cser_read(FILE* file, char* buffer, size_t blen, char** name, char** str_value, long* num_value) {
	size_t bpos = 0, inpos = 0; /* buffer positions */
	char inbuf[512]; /* temporary input buffer */
	char esc = 0; /* are we in escape mode? */

	/* init */
	*name = NULL;
	*str_value = NULL;
	*num_value = 0;

	/* eof? */
	if (feof(file))
		return -1;

	/* loop until eof */
	while (fgets(inbuf, sizeof(inbuf), file) != NULL) {
		/* copy line into buffer, undoing escaping as we go */
		for (inpos = 0; inbuf[inpos] != 0 && inbuf[inpos] != '\n'; ++inpos) {
			/* have we any space left in buffer? */
			if (bpos < blen - 1) {
				/* escaped? */
				if (esc) {
					/* determine type */
					switch(inbuf[inpos]) {
						case '\\': buffer[bpos++] = '\\'; break;
						case 'n': buffer[bpos++] = '\n'; break;
						case '0': buffer[bpos++] = '\0'; break;
						default: break; /* just lose the erroneous escape */
					}
					esc = 0;
				} else 
				/* begin escape? */
				if (inbuf[inpos] == '\\') {
					esc = 1;
				} else
				/* plain text */
				{
					buffer[bpos++] = inbuf[inpos];
				}
			}
		}

		/* break on EOL */
		if (inbuf[inpos] == '\n')
			break;
	}

	/* NUL terminate buffer */
	buffer[bpos] = 0;

	/* find the name/value separator */
	*name = buffer;
	*str_value = strchr(buffer, '=');

	/* is there a value? */
	if (*str_value != NULL) {
		/* break it up properly; NUL terminate name, move forward str value */
		**str_value = 0;
		++ *str_value;

		/* make integer version */
		*num_value = atol(*str_value);
	}

	return 0;
}

int cser_pkstr(char *str){
	int size = strlen(str)+1;

	pvm_pkint(&size, 1, 1);
	pvm_pkbyte(str, size, 1);

	return 0;
}

char *cser_upkstr() {
	int size = -1;

	pvm_upkint(&size, 1, 1);
	char *buf = malloc(sizeof(char)*size);
	pvm_upkbyte(buf, size, 1);

	return buf;
}



int sendStruct(int tid, int tag, void *dataStruct, packData_t *packer) {
	pvm_initsend(PvmDataDefault);
	packer(dataStruct);
	pvm_send(tid, tag);
	
	return 0;
}

int mcastStruct(int *tids, int tidListSize, int tag, void *dataStruct, packData_t *packer) {
	pvm_initsend(PvmDataDefault);
	packer(dataStruct);
	pvm_mcast(tids, tidListSize, tag);
	
	return 0;
}

void *recvStruct(int tid, int tag, unpackData_t *unpacker){ 
	pvm_recv(tid, tag);
	
	return unpacker();
}

