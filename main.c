#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "cJSON.h"


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
	printf("Okay\n");
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
		memset(input,0 ,sizeof(input));	
		scanf("%40s", input);
		if(strlen(input)==1 // Only one character
		   && input[0] == 'q'){
			flag = false;
			break;
		}
		if(strncmp(input, "new", 3) != 0){
			printf("Works\n");
			char task[255];
			
			if (strlen(input) > 3) {
    				stringTillEnd(input, 3, task);
			}

			printf("%s", task);
			char key[125];
			sprintf(key,"%d",keyNumber),
			cJSON_AddItemToObject(json, key, cJSON_CreateString(task));
			keyNumber++;
		}
	}
	cJSON_ReplaceItemInObjectCaseSensitive(json, "keyNumber",  cJSON_CreateNumber(keyNumber));
	writeToJSON(json);
	return 0;
}

