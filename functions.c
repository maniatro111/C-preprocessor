#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief This function checks if a selected word starting at pos
 * is or is not between double quotes
 * @param pos - The position at which the word starts
 * @param nr_el - Number of elements in the double quotes position array
 * @param vec - This array stores the positions of all pairs of double quotes in
 * that line
 * @return 1 - if the word is not between double quotes
 *         0 - if the word is between double quotes
 */
static int check_not_in_between(int pos, int nr_el, int *vec)
{
	int i;

	if (!nr_el)
		return 1;
	for (i = 0; i < nr_el; i += 2)
		/* If the start of the word is between two double quotes, return
		 * 0 */
		if (pos >= vec[i] && pos <= vec[i + 1])
			return 0;
	return 1;
}

/**
 * @brief This function gets all the positions of the double quotes
 * present in the given line
 * @param string - The line in which we search for the double quotes
 * @param vect - The array in which we store the positions of the double quotes
 * found in the line
 * @param el_number - the size of the vect array
 * @return 12 - if there is a memory allocation problem
 *         0 - if the function is successful
 */
static int get_double_quotes(char *string, int **vect, int *el_number)
{
	/* Initialize the number of double quotes to 0 */
	*el_number = 0;
	/* If we find double quotes in the string */
	if (strchr(string, '\"')) {
		int initial_size;

		initial_size = 10;
		*vect = (int *)malloc(initial_size * sizeof(int));
		if ((*vect) == NULL)
			return 12;
		/* Get the position of the first double quotes symbol */
		(*vect)[*el_number] = strchr(string, '\"') - string;
		/* Move a position to the right to store the next position */
		(*el_number)++;
		/* While we still find double quotes in the line, store their */
		/* position */
		while (1 + (*vect)[(*el_number) - 1] < strlen(string) &&
		       strchr(string + 1 + (*vect)[(*el_number) - 1], '\"')) {
			(*vect)[*el_number] =
			    strchr(string + 1 + (*vect)[(*el_number) - 1],
				   '\"') -
			    string;
			(*el_number)++;
		}
	}
	return 0;
}

/**
 * @brief This function searches for a key in the map and copies the value
 * at that key into the line that needs to be interpreted
 * @param mp - The map in which we have to search the word
 * @param value - The line that we have to interpret
 * @param start - The starting position of the word
 * @param end - The ending position of the word
 * @return 12 - if there is a memory problem
 *         0 - if there is no error
 */
static int search_for_key_and_copy(map *mp, char **value, int *start, int *end)
{
	char *aux;

	aux = (char *)calloc((*end - *start + 1), sizeof(char));
	if (aux == NULL)
		return 12;

	/* Copy in aux the possible word */
	strncpy(aux, (*value) + *start, *end - *start);
	strcat(aux, "\0");

	/* Check if you have the word in the map */
	if (map_get(mp, aux)) {
		char *valoare;
		char *copie;

		/* Get the value that coresponds to the key */
		valoare = map_get(mp, aux);
		copie = (char *)calloc(
		    (strlen(*value) - strlen(aux) + strlen(valoare) + 1),
		    sizeof(char));
		if (copie == NULL) {
			free(aux);
			return 12;
		}

		/* Copy all the words before the word that we analyze */
		strncpy(copie, *value, *start);
		/* Append the value of the key */
		strcat(copie, valoare);
		/* Append the rest of the line, jumping the word that we */
		/* exchanged */
		strcat(copie, (*value) + *end);
		/* Append the null terminated string */
		strcat(copie, "\0");
		/* The new end from which we continue to analyze words */
		*end = *start + strlen(valoare);
		/* Free the old string */
		free(*value);
		/* Replace the old string with the new one */
		*value = copie;
	} else
		/* If is not in the map, modify the end of the word */
		*end = *start + strlen(aux);
	/* Free the aux string */
	free(aux);
	/* Make the start end + 1 to point to the same character after the next
	 */
	/* for iteration */
	*start = *end + 1;
	return 0;
}

/**
 * @brief This function splits the words in the line read from the file
 * and checks if each work needs to be replaced
 * @param mp - The map in which we search for the key
 * @param value - The line read from the file
 * @return the value of the function search_for_key_and_copy
 */
static int check_define_in_define(map *mp, char **value)
{
	int start = 0;
	int end = 0;
	int return_value = 0;

	/* Iterate through the whole line till the end of it, or till we */
	/* encounter a memory problem */
	for (; end < strlen(*value) && return_value == 0; end++)
		/* If we encounter one of the character in the list, that means
		 */
		/* that we found th end of a word. Start analyzing it. */
		if (strchr("\t []{}<>=+-*/%!&|^.,:;()\\", (*value)[end]))
			return_value =
			    search_for_key_and_copy(mp, value, &start, &end);
	/* Analyze the last word */
	if (return_value == 0)
		return_value = search_for_key_and_copy(mp, value, &start, &end);
	return return_value;
}

/**
 * @brief This function eliminates all tabs or spaces at the begining of the
 * line, replaces them with a space and copies the rest of the string in another
 * @param dest - destination string
 * @param source - source string
 */
static void eliminate_tabs_and_add_space(char *dest, char *source)
{
	int i = 0;

	while (i < strlen(source) && (source[i] == ' ' || source[i] == '\t'))
		i++;
	strcat(dest, " ");
	strcat(dest, source + i);
}

/**
 * @brief This function inserts all the defines read from a file
 * @param mp - The map in which we need to add the macros
 * @param buf - The line read from the file
 * @param infd - File* to the file we are reading from (in case of multi-line
 * defines)
 * @return 0 - If the function finishes successfully
 *         12 - If there is a memory allocation problem
 */
static int insert_define_from_file(map *mp, char *buf, FILE *infd)
{
	char *argumente;
	char *aux;
	char *key;
	int return_value = 0;

	/* Copy the whole line in an aux */
	argumente = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	if (argumente == NULL)
		return 12;
	strcpy(argumente, buf);
	/* Split the definition and the value of the macro */
	aux = strtok(argumente, "\n ");
	key = (char *)malloc((strlen(aux) + 1) * sizeof(char));
	if (key == NULL) {
		free(argumente);
		return 12;
	}
	strcpy(key, aux);
	aux = strtok(NULL, "\n");
	/* If the macro has no value, insert it with the empty string */
	if (aux == NULL)
		return_value = map_set(mp, key, "");
	/* Else, check if it is a multi-line define */
	else if (aux[strlen(aux) - 1] == '\\') {
		char *argumente_concatenate;
		int i;
		char *mimi;
		/* Declare an auxiliary string */
		argumente_concatenate = (char *)calloc(250, sizeof(char));
		if (argumente_concatenate != NULL)
			strcat(argumente_concatenate, aux);
		/* Erase the \character at the end */
		argumente_concatenate[strlen(argumente_concatenate) - 1] = '\0';
		/* Erase all the tabs and spaces at the end */
		for (i = strlen(argumente_concatenate) - 1;
		     i >= 0 && (argumente_concatenate[i] == ' ' ||
				argumente_concatenate[i] == '\t');
		     i--)
			argumente_concatenate[i] = '\0';
		/* Read the next line */
		fgets(buf, MAX_CMD_BUF_SIZE, infd);
		/* Erase the \n character from the end */
		buf[strlen(buf) - 1] = '\0';
		/* Loop and read lines till we find a line that doesn't end with
		 */
		/* \ anymore. That means that we finised reading the multi-line
		 */
		/* define */
		while (buf[strlen(buf) - 1] == '\\') {
			/* Erase all the tabs and spaces of the new read line */
			/* and append it to the value of the define */
			eliminate_tabs_and_add_space(argumente_concatenate,
						     buf);
			/* Erase the \ character at the end of the line */
			argumente_concatenate[strlen(argumente_concatenate) -
					      1] = '\0';
			/* Erase all the spaces and tabs at the end of the */
			/* string */
			for (i = strlen(argumente_concatenate);
			     i >= 1 && (argumente_concatenate[i] == ' ' &&
					argumente_concatenate[i - 1] == ' ');
			     i--)
				argumente_concatenate[i] = '\0';
			/* Read another line */
			fgets(buf, MAX_CMD_BUF_SIZE, infd);
			/* Erase the \n at the end of the line */
			buf[strlen(buf) - 1] = '\0';
		}
		/* For the last line of the define, elimitate all starting tabs
		 */
		/* and spaces, the copy it to the final value */
		eliminate_tabs_and_add_space(argumente_concatenate, buf);
		mimi = (char *)malloc((strlen(aux) + 1) * sizeof(char));
		if (mimi == NULL)
			return_value = 12;
		if (return_value == 0) {
			strcpy(mimi, aux);
			/* Check that the value does not contain other macros */
			return_value = check_define_in_define(mp, &mimi);
		}
		if (return_value == 0)
			/* Add the macro to the map */
			return_value = map_set(mp, key, argumente_concatenate);
		if (return_value == 0)
			free(mimi);
		free(argumente_concatenate);
		/* Else, insert it in the map. Before that, make sure that the
		 */
		/* value hasn't any other macros defined in it. */
	} else {
		char *mimi;

		mimi = (char *)malloc((strlen(aux) + 1) * sizeof(char));
		if (mimi == NULL)
			return_value = 12;
		if (return_value == 0) {
			strcpy(mimi, aux);
			/* Check that the value does not contain other macros */
			return_value = check_define_in_define(mp, &mimi);
		}
		if (return_value == 0)
			/* Add the macro to the map */
			return_value = map_set(mp, key, mimi);
		if (return_value == 0)
			free(mimi);
	}
	/* Free all the resources */
	free(key);
	free(argumente);
	return return_value;
}

/**
 * @brief This function gets a normal line read from the file,
 * analizes it, replaces macros and prints it
 * @param map - The map in which the macros are stored
 * @param buf - The line read form the file
 * @param outfd - The file pointre to the output
 * @return 0 - If the function runs successfully
 *             the result of the search_for_key_and_copy
 */
static int analyze_and_print(map *map, char *buf, FILE *outfd)
{
	int *v;
	int n;
	char *copie;
	int start = 0;
	int end = 0;
	int return_value = 0;

	copie = (char *)calloc(MAX_CMD_BUF_SIZE, sizeof(char));
	if (copie == NULL)
		return 12;
	strcpy(copie, buf);
	/* Get all the double quotes that are in the line */
	return_value = get_double_quotes(buf, &v, &n);

	/* Iterate through the whole line */
	for (; end < strlen(copie) && return_value == 0; end++)
		/* If the current character is one of these, that means that we
		 */
		/* found the end of a word */
		if (strchr("\t []{}<>=+-*/%!&|^.,:;()\\", (copie)[end])) {
			/* Check that the start of the word isn't between double
			 */
			/* quotes */
			if (check_not_in_between(start, n, v))
				/* Check if the word is a macro and needs to be
				 */
				/* changed */
				return_value = search_for_key_and_copy(
				    map, &copie, &start, &end);
			else
				/* If the word isn't a macro, jump to the next
				 */
				/* word */
				start = end + 1;
		}
	/* Check that the last word isn't between double quotes */
	if (return_value == 0 && check_not_in_between(start, n, v))
		/* Check if the last word is a macro and needs to be */
		/* changed */
		return_value =
		    search_for_key_and_copy(map, &copie, &start, &end);
	/* If the first character is a tab, exchange it with a space */
	if (return_value == 0) {
		if (copie[0] == '\t')
			copie[0] = ' ';
		/* Print the line */
		fprintf(outfd, "%s", copie);
		/* Free the double quotes array if it was used */
		if (n)
			free(v);
	}
	/* Free the resources */
	free(copie);
	return return_value;
}

/**
 * @brief This function adds a new macro given from the
 * command line into the map
 * @param argv - all the arguments given to the executable
 * @param line - current line in the argument matrix
 * @param map - The map in which we want to add the macro
 * @return 0 - if the function is successful
 *             the result of map_set
 */
int add_argument_mapping(char **argv, int *line, map *map)
{
	int return_value = 0;

	/* If there is no space between the -D option and macro, move two bytes
	 */
	/* to the right */
	if (strlen(argv[*line]) > 2)
		argv[*line] = argv[*line] + 2;
	else
		/* Else, go to the next line */
		(*line)++;
	/* If there is a = character, that means that the macro also has a value
	 */
	if (strchr(argv[*line], '=') != NULL) {
		/* Split the macro name and the value */
		strtok(argv[*line], "=");
		/* Add an entry to the map */
		return_value = map_set(map, argv[*line],
				       argv[*line] + strlen(argv[*line]) + 1);
	} else
		/* Add an entry with empty value string */
		return_value = map_set(map, argv[*line], "");
	return return_value;
}

/**
 * @brief This function erases an entry from the map given as argument
 * @param map - The map that probably contains the key
 * @param key - The key of the entry that we want to erase
 */
static void undefine_key(map *map, char *key)
{
	key[strlen(key) - 1] = '\0';
	delete_entry(map, key);
}

/**
 * @brief This function turns an expression ( 0 or 1 )
 * to int and evaluates it.
 * @param eval - expression given as string
 * @return 0 - if the expression reduces to 0
 *         1 - if the expression reduces to 1
 */
static int turn_to_int_and_check(char *eval)
{
	int a;

	a = atoi(eval);
	if (a == 0)
		return 0;
	else
		return 1;
}

/**
 * @brief This function replaces all the macros in the expression before
 * evaluating it
 * @param map - The map in which we search for macros
 * @param key - The macro
 * @return the result of turn_to_int_and_check
 */
static int evaluate_if_condition(map *map, char *key)
{
	char *aux;

	/* Erase the \n of the line */
	key[strlen(key) - 1] = '\0';
	/* Get the value that may be at the key; */
	aux = map_get(map, key);
	/* If the value is NULL then it may already be an expression that we can
	 */
	/* evaluate. Evaluate it.*/
	if (aux == NULL)
		return turn_to_int_and_check(key);
	/* Else, copy the value of the key and evaluate it */
	strcpy(key, aux);
	return turn_to_int_and_check(key);
}

/**
 * @brief Check if a macro is defined (not if it also has a value)
 * @param map - The map in which we search the macro
 * @param key - The macro we are searching for
 * @return 0 - if the macro is not defined
 *         1 - if the macro is defined
 */
static int check_if_defined(map *map, char *key)
{
	key[strlen(key) - 1] = '\0';
	return key_exists(map, key);
}

/**
 * @brief This function checks if a file is in a directory
 * given as argument
 * @param directory_list - the list of paths to the directory
 * @param directory_list_size - the size of the list
 * @param file - the name of the file
 * @return FILE* of the file found in one of the directories
 *         NULL if the file isn't in any directory
 */
static FILE *check_if_file_in_dir(char **directory_list,
				  int directory_list_size, char *file)
{
	int i;
	/* Iterate through all the possible paths given as argument */
	for (i = 0; i < directory_list_size; i++) {
		char *path;
		FILE *fd;

		path = (char *)calloc(
		    (strlen(directory_list[i]) + strlen(file) + 2),
		    sizeof(char));
		if (path == NULL)
			return NULL;
		/* Append the name of the file to the current path */
		strcpy(path, directory_list[i]);
		strcat(path, "/");
		strcat(path, file);
		strcat(path, "\0");
		/* Try to open the file */
		fd = fopen(path, "r");
		free(path);
		/* If the file opened, return the FILE* */
		if (fd)
			return fd;
	}
	/* Else the file does not exist in all the directories given as */
	/* argument. Return NULL. */
	return NULL;
}

/**
 * @brief This function searches for the file that needs to be included
 * by a #include directive and starts to analyze it
 * @param buf - the name of the file that needs to be included
 * @param directory_list - all the paths to the directories that
 * probably contain the file
 * @param outfd - File* to output file
 * @param map - map with the macros
 * @param directory_list_size - the size of the directory list
 * @param relative_path - the relative path to the input file
 * @return 12 if there are memory problems or the file does not exist
 *         the result of the read_file function
 */
static int add_header_file(char *buf, char **directory_list, FILE *outfd,
			   map *map, int directory_list_size,
			   char *relative_path)
{
	char *total_path;
	FILE *infd;
	FILE *fd;
	int return_value = 0;

	/* Remove the \n character and the ending double quotes */
	buf[strlen(buf) - 1] = '\0';
	buf[strlen(buf) - 1] = '\0';

	total_path =
	    (char *)calloc((strlen(relative_path) + strlen(buf)), sizeof(char));
	if (total_path == NULL)
		return 12;
	/* Try and see if the header file is in the same directory as the input
	 */
	/* file */
	strcpy(total_path, relative_path);
	strcat(total_path, buf + 1);
	infd = fopen(total_path, "r");
	/* If it is, start to analyze it */
	if (infd) {
		return_value = read_file(map, infd, outfd, directory_list,
					 directory_list_size, relative_path);
		fclose(infd);
		free(total_path);
		return return_value;
	}
	free(total_path);
	/* Else check if the header file is in one of the directories given as
	 */
	/* argument */
	fd = check_if_file_in_dir(directory_list, directory_list_size, buf + 1);
	if (fd) {
		return_value = read_file(map, fd, outfd, directory_list,
					 directory_list_size, relative_path);
		fclose(fd);
		return return_value;
	}
	/* Else return an error */
	return 12;
}

/**
 * @brief This function reads a file line by line and analizes it. It
 * checks for all the guards tested by this homework.
 * @param map - the map where all the macros are stored
 * @param infd - File* to the input file
 * @param outfd - File* to the output file
 * @param directory_list - list of paths to directories
 * @param directory_list_size - the size of the list
 * @param relative_path - relative path of the input file
 * @return 0 - if everything ends successfully
 *         12 - if there was a memory problem
 */
int read_file(map *map, FILE *infd, FILE *outfd, char **directory_list,
	      int directory_list_size, char *relative_path)
{
	char buf[MAX_CMD_BUF_SIZE];
	int return_value = 0;
	/* This value is used to see if I can interpret the line that I read. */
	/* When I am in a if/else block an I have to interpret only one of it */
	/* this value will be 1 for the part that I have to interpret and 0 for
	 */
	/* the part that I cannot interpret. */
	int allowed_to_interpret = 1;

	/* While there are lines to be read */
	while (fgets(buf, MAX_CMD_BUF_SIZE, infd) && return_value == 0) {
		/* If I can interpret the line and there wasn't an error */
		if (allowed_to_interpret == 1 && return_value == 0) {
			/* If the line is a define */
			if (strncmp(buf, "#define", 7) == 0)
				return_value =
				    insert_define_from_file(map, buf + 8, infd);
			/* If the line is an undefine */
			else if (strncmp(buf, "#undef", 6) == 0)
				undefine_key(map, buf + 7);
			/* If the line is an ifdef block */
			else if (strncmp(buf, "#ifdef", 6) == 0)
				allowed_to_interpret =
				    check_if_defined(map, buf + 7);
			/* If the line is an ifndef block */
			else if (strncmp(buf, "#ifndef", 7) == 0)
				allowed_to_interpret =
				    (check_if_defined(map, buf + 8) + 1) % 2;
			/* If the line is an if block */
			else if (strncmp(buf, "#if", 3) == 0)
				allowed_to_interpret =
				    evaluate_if_condition(map, buf + 4);
			/* If the line is an else block, make the semaphore 0 so
			 */
			/* that I don't interpret the next lines. */
			else if (strncmp(buf, "#else", 5) == 0)
				allowed_to_interpret = 0;
			/* If the line is an include*/
			else if (strncmp(buf, "#include", 8) == 0)
				return_value = add_header_file(
				    buf + 9, directory_list, outfd, map,
				    directory_list_size, relative_path);
			/* If the line is an endif*/
			else if (strncmp(buf, "#endif", 6) == 0)
				;
			/* If the line is consists of only one enter */
			else if (strlen(buf) == 1 && buf[0] == '\n')
				;
			else
				/* If the line is not a guard, analyze the */
				/* line */
				return_value =
				    analyze_and_print(map, buf, outfd);
			/* These commands need to be interpreted even if the */
			/* semaphore is on 0 */
		} else if (strncmp(buf, "#else", 5) == 0)
			/* Make the semaphore 1 to start interpreting next lines
			 */
			allowed_to_interpret = 1;
		else if (strncmp(buf, "#elif", 5) == 0)
			/* Evaluate the elif expression and make the semaphore
			 */
			/* according to the result */
			allowed_to_interpret =
			    evaluate_if_condition(map, buf + 6);
		/* If we reached the end of an if block make the semaphore 1 */
		else if (strncmp(buf, "#endif", 6) == 0)
			allowed_to_interpret = 1;
	}
	return return_value;
}

/**
 * @brief This function adds a path to a directory to the directory
 * array
 * @param list - array to all the directory paths
 * @param capacity - the array capacity
 * @param size - the number of paths stored
 * @param path - the path that needs to be added
 * @return 0 - if the function finishes successfully
 *         12 - if there is a memory problem
 */
int add_directory_path(char ***list, int *capacity, int *size, char *path)
{
	/* If it is the first path that needs to be added, initialize the string
	 */
	/* array */
	if ((*capacity) == 0) {
		*capacity = 10;
		(*list) = (char **)malloc((*capacity) * sizeof(char *));
		if ((*list) == NULL)
			return 12;
		/* Else, if we reached the capacity of the array, resize it and
		 */
		/* copy the new entry */
	} else if (*size == *capacity) {
		*capacity += 10;
		(*list) = (char **)realloc(*list, (*capacity) * sizeof(char *));
		if ((*list) == NULL)
			return 12;
	}
	(*list)[*size] = (char *)malloc((strlen(path) + 1) * sizeof(char));
	if ((*list)[*size] == NULL) {
		free((*list));
		return 12;
	}
	strcpy((*list)[*size], path);
	/* Increase the number of entries with 1 */
	(*size)++;

	return 0;
}

/**
 * @brief This function extracts the relative path from the input file
 * and stores it in the relative_path pointer
 * @param buf - full path that contains the input file
 * @param relative_path - the relative path extracted from the input
 * file
 * @return 0 - if the function finishes successfully
 *         12 - if there is a memory problem
 */
int get_relative_path(char *buf, char **relative_path)
{
	int k = strlen(buf);
	/* From the end of the string, iterate till the beginning of it or till
	 */
	/* we find a / character. If we find this character, that means that the
	 */
	/* input file has a relative path. */
	k--;
	for (; k >= 0 && buf[k] != '/'; k--)
		;
	if (k > 0) {
		/* If it has a relative path, copy and store it */
		(*relative_path) = (char *)calloc((k + 2), sizeof(char));
		if ((*relative_path) == NULL)
			return 12;
		strncpy((*relative_path), buf, k + 1);
		strcat((*relative_path), "\0");
	}
	return 0;
}

/**
 * @brief This function copies the file name given in buf to the
 * infile_name
 *
 * @param infile_name - the pointer where the name will be copied too
 * @param buf - the file name that needs to be copied
 * @return 0 - if the function finishes successfully
 *         12 - if there is a memory problem
 */
int copy_file_name(char **infile_name, char *buf)
{
	(*infile_name) = (char *)malloc((strlen(buf) + 1) * sizeof(char));
	if ((*infile_name) == NULL)
		return 12;
	strcpy((*infile_name), buf);
	return 0;
}

/**
 * @brief This function frees all the memory allocated for storing
 * directory paths
 * @param directory_list - the array of directory paths
 * @param size - the size of the array
 */
void free_directory_list(char **directory_list, int size)
{
	if (size != 0) {
		int i;

		for (i = 0; i < size; i++)
			free(directory_list[i]);
		free(directory_list);
	}
}

/**
 * @brief This function opens a file and returns the FILE* to it
 *
 * @param name - the path to the file that we want to open
 * @param fd - FILE* of the file that was opened
 * @param mode - the mode in which we want to open the file ("r" or
 * "w+")
 * @return -1 - if the file fails to open
 *         0 - if the function finishes successfully
 */
int open_file(char *name, FILE **fd, char *mode)
{
	if (name != NULL) {
		*fd = fopen(name, mode);
		if (*fd == NULL)
			return -1;
	}
	return 0;
}

/**
 * @brief This function closes a file, erases it's name and the relative
 * path
 *
 * @param name - the path to the file that was opened
 * @param fd - FILE* of the file
 * @param relative_path - the relative path extracted and stored
 * separatelly
 */
void close_file(char *name, FILE *fd, char *relative_path)
{
	if (name != NULL) {
		if (relative_path != NULL)
			free(relative_path);
		free(name);
		fclose(fd);
	}
}
