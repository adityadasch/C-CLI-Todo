#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lib/cJSON.h"
#include <stdlib.h>

void readFromJSON(char *buffer, size_t size){
	FILE *fp = fopen("task.json", "r");
	if(fp==NULL){
		printf("Unable to find file");
	}
	fread(buffer, 1, size, fp);
	fclose(fp);
}

void writeToJSON(cJSON *json){
	char *data = cJSON_Print(json);
	FILE *fp = fopen("task.json", "w");
	if(fp==NULL){
		printf("Unable to find file");
	}
	fputs(data, fp);
	fclose(fp);
}

void stringTillEnd(char *src, int offset, char *out){
	strcpy(out, src+offset);
}

void createTask(char *input, int *keyNumber, cJSON *json){
	char task[strlen(input)+3];
	if (strlen(input) > 4) { // The input is more than "new "
			stringTillEnd(input, 4, task); // Remove the task trailing space
	}
	char key[125];
	sprintf(key,"%d",(*keyNumber));
	cJSON *array = cJSON_CreateArray();
	cJSON_AddItemToArray(array, cJSON_CreateFalse());
	cJSON_AddItemToArray(array, cJSON_CreateString(task));

	cJSON_AddItemToObject(json, key, array);
	(*keyNumber)++;
}

int readLineFromStdin(char *buf, size_t size) {
	printf(">");
    if (fgets(buf, size, stdin) == NULL) {
        return 1; // error or EOF
    }
    buf[strcspn(buf, "\n")] = '\0'; // trim newline

    // flush leftovers if truncated
    if (strlen(buf) == size - 1 && buf[size - 2] != '\n') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    return 0;
}



int main(){
	char buffer[1024];
	readFromJSON(buffer, 1024);
	cJSON *json = cJSON_Parse(buffer);
	if (json==NULL){
		cJSON_Delete(json);
		return 1;
	}
	int keyNumber = cJSON_GetObjectItemCaseSensitive(json, "keyNumber")->valueint;
	bool flag = true;
	char input[255];

	while(flag){
		if(readLineFromStdin(input, sizeof(input))==1){
			continue;
		}
		if(strlen(input)==1 // Only one character
		   && input[0] == 'q'){
			flag = false;
			break;
		} else if(strncmp(input, "new", 3) == 0){
			createTask(input, &keyNumber, json);
		}
	cJSON_ReplaceItemInObjectCaseSensitive(json, "keyNumber",  cJSON_CreateNumber(keyNumber));
	writeToJSON(json);
	return 0;
}

