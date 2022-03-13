#include "hash_table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void insert_define_from_file(ht* tabel, char* buf, FILE* infd){
    char* argumente = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	strcpy(argumente, buf);
	char *aux = strtok(argumente, " \n");
	char *key = (char *) malloc((strlen(aux) + 1) * sizeof(char));
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	if (aux[strlen(aux) - 1] == '\\'){
	    char* argumente_concatenate = (char *)malloc(250 * sizeof(char));
		strcat(argumente_concatenate, aux);
		argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';
		int i;
		for (i = strlen(argumente_concatenate) - 1 ; i >= 0 && (argumente_concatenate[i] == ' ' || argumente_concatenate[i] == '\t'); i--)
			argumente_concatenate[i] = '\0';
		fgets(buf, 256, infd);
		buf[strlen(buf) - 1] = '\0';
		while(buf[strlen(buf) - 1] == '\\'){
			int i;
			for(i = 0; i < strlen(buf) && (buf[i] == ' ' || buf[i] == '\t'); i++);
			strcat(argumente_concatenate, " ");
			strcat(argumente_concatenate, buf + i);
			argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';

			for (i = strlen(argumente_concatenate); i >= 1 && (argumente_concatenate[i] == ' ' && argumente_concatenate[i - 1] == ' '); i--)
				argumente_concatenate[i] = '\0';
			fgets(buf, 256, infd);
			buf[strlen(buf) - 1] = '\0';
		}
	    ht_set(tabel, key, argumente_concatenate);
		free(argumente_concatenate);
	}
	else
		ht_set(tabel, key, aux);
	free(key);
	free(argumente);
}