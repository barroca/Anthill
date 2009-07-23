#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "cser.h"

typedef long ID;
struct TestStruct_t {
	ID id;
	char* name;
	int hp;
	int hp_max;
};
typedef struct TestStruct_t TestStruct;

void
save_test(TestStruct* data) {
	FILE* file;

	file = fopen("test.txt", "w");

	cser_write_num(file, "id", data->id);
	cser_write_str(file, "name", data->name);
	cser_write_num(file, "hp", data->hp);
	cser_write_num(file, "maxhp", data->hp_max);

	fclose(file);
}

TestStruct*
load_test(void) {
	FILE* file;
	TestStruct* data;

	data = (TestStruct*)malloc(sizeof(TestStruct));
	if (data == NULL)
		return NULL;
	memset(data, 0, sizeof(TestStruct));

	file = fopen("test.txt", "r");
	if (file == NULL) {
		free (data);
		return NULL;
	}

	CSER_READ_BEGIN(file)
		CSER_READ_STR("name", data->name);
		CSER_READ_NUM("id", data->id);
		CSER_READ_NUM("hp", data->hp);
		CSER_READ_NUM("maxhp", data->hp_max);
	CSER_READ_END

	return data;
}

void
print_test (TestStruct* data) {
	printf("ID: %ld\n", data->id);
	printf("Name is '%s'\n", data->name);
	printf("HP: %d/%d\n", data->hp, data->hp_max);
}

int
main (void) {
	TestStruct* data;

	/* save */
	printf("Saving...\n");
	data = (TestStruct*)malloc(sizeof(TestStruct));
	if (data == NULL) {
		fprintf(stderr, "malloc(): %s\n", strerror(errno));
		return 1;
	}
	data->id = 500;
	data->name = "Elanthis";
	data->hp = 70;
	data->hp_max = 75;
	print_test(data);
	save_test(data);
	free(data);

	/* load */
	printf("Loading...\n");
	data = load_test();
	if (data == NULL) {
		fprintf(stderr, "load_data(): %s\n", strerror(errno));
		return 1;
	}
	print_test(data);
	free(data);

	return 0;
}
