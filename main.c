// lib/cJSON
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "lib/cJSON.h"
#include <stdlib.h>

void printHelp(void){
    printf(
        "\n"
        "Todo CLI Commands\n"
        "=================\n\n"
        "new <task>\n"
        "    Create a new task.\n\n"
        "done <id>\n"
        "d <id>\n"
        "    Mark a task as completed.\n\n"
        "todo <id>\n"
        "t <id>\n"
        "    Mark a task as not completed.\n\n"
        "list | l\n"
        "    List all tasks.\n\n"
        "help\n"
        "    Show this help message.\n\n"
        "q\n"
        "    Quit the program.\n\n"
        "clearalltasks\n"
        "    Delete all tasks and reset the database.\n\n"
    );
}
void readFromJSON(char *buffer, size_t size){
	FILE *fp = fopen("task.json", "r");
	if(fp==NULL){
		printf("Unable to find file");
	}
	fread(buffer, 1, size, fp);
	fclose(fp);
}

void clearJSON(){
	printf("Cleared");
	char *data = "{\"keyNumber\":0}";
	FILE *fp = fopen("task.json", "w");
	if(fp==NULL){
		printf("Unable to find file");
	}
	fputs(data, fp);
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

void createTask(cJSON *json, char *input, int *keyNumber){
	char task[strlen(input)+3];
	if (strlen(input) > 4) { // The input is more than "new "
			stringTillEnd(input, 4, task); // Remove the task trailing space
	}
	char key[125];
	sprintf(key,"%d",(*keyNumber+1));
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

void markAsDone(cJSON *json, char *input, bool isShort){
	char task[strlen(input)+3];
	if (strlen(input) > 4 && !isShort) { // The input is more than "new "
		stringTillEnd(input, 4, task); // Remove the task trailing space
	} else if (strlen(input) > 2 && isShort){ // The input is more than "n "
		stringTillEnd(input, 2, task); // Remove the task trailing space
	}
	char key[128];
	strcpy(key, task);
	cJSON *info = cJSON_GetObjectItem(json, key);
	if(info!=NULL){
		cJSON_ReplaceItemInArray(info, 0, cJSON_CreateTrue());
	}
}

void markAsUnDone(cJSON *json, char *input, bool isShort){
	char task[strlen(input)+3];
	if (strlen(input) > 4 && !isShort) { // The input is more than "new "
		stringTillEnd(input, 4, task); // Remove the task trailing space
	} else if (strlen(input) > 2 && isShort){ // The input is more than "n "
		stringTillEnd(input, 2, task); // Remove the task trailing space
	}
	char key[128];
	strcpy(key, task);
	cJSON *info = cJSON_GetObjectItem(json, key);
	if(info!=NULL){
		cJSON_ReplaceItemInArray(info, 0, cJSON_CreateFalse());
	}
}


void listTasks(cJSON *json){
	cJSON *array_item = NULL;
	int count = 1;
	cJSON_ArrayForEach(array_item, json){
		if(strcmp(array_item->string,"keyNumber")==0) continue;
		bool status = cJSON_IsTrue(cJSON_GetArrayItem(array_item, 0));
		char* task = cJSON_GetArrayItem(array_item,1)->valuestring;
		printf("%d|[", count);
		if (status) printf("x]");
		else printf(" ] ");
		printf("%s\n", task);
		count++;
	} 
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
		} else if(strncmp(input, "help", 4) == 0) {
			printHelp();
		} else if(strncmp(input, "new", 3) == 0){
			createTask(json, input, &keyNumber);
		} else if(strncmp(input, "done", 4) == 0 || (input[0] == 'd' && input[1]!='o')){
			markAsDone(json, input, strncmp(input, "mark", 4) != 0);  
		} else if(strncmp(input, "todo", 4) == 0 || (input[0] == 't' && input[1]!='o')){
			markAsUnDone(json, input, strncmp(input, "umak", 4) != 0); 
		} else if(strncmp(input, "list", 4) == 0 || input[0] == 'l'){
			listTasks(json); 
		} else if(strncmp(input, "clearalltasks",13) == 0) {clearJSON();return 0;}
	}
	cJSON_ReplaceItemInObjectCaseSensitive(json, "keyNumber",  cJSON_CreateNumber(keyNumber));
	writeToJSON(json);
	return 0;

}
