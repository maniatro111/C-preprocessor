#include "every_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef HASH_TABLE
#define HASH_TABLE 1
#include "hash_table.h"
#endif

char *infile;
char *outfile;
FILE *infd, *outfd;
ht *map;

int main(int argc, char **argv)
{
	int i;
	char buf[256];

	infile = NULL;
	infd = stdin;
	outfd = stdout;
	outfile = NULL;

	map = ht_create();
	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'D') {
				if (strlen(argv[i]) > 2)
					argv[i] = argv[i] + 2;
				else
					i++;

				if (strchr(argv[i], '=') != NULL) {
					char *aux;

					aux = strtok(argv[i], "=");
					ht_set(map, argv[i],
					       argv[i] + strlen(argv[i]) + 1);
				} else {
					ht_set(map, argv[i], "");
				}
			} else if (argv[i][1] == 'I') {
			} else if (argv[i][1] == 'o') {
			} else {
				return 1;
			}
		} else {
			if (infile == NULL) {
				infile = (char *)malloc((strlen(argv[i]) + 1) *
							sizeof(char));
				strcpy(infile, argv[i]);
			} else {
				if (outfile == NULL) {
					outfile = (char *)malloc(
					    (strlen(argv[i]) + 1) *
					    sizeof(char));
					strcpy(outfile, argv[i]);
				} else {
					return 1;
				}
			}
		}
	}

	if (infile != NULL) {
		infd = fopen(infile, "r");
		if (infd == NULL)
			return 1;
	}
	if (outfile != NULL) {
		outfd = fopen(outfile, "w+");
		if (outfd == NULL)
			return 1;
	}

	while (fgets(buf, 256, infd)) {
		if (buf[0] == '#') {
			if (strncmp(buf + 1, "define", 6) == 0) {
				insert_define_from_file(map, buf + 8, infd);
			} else if (strncmp(buf + 1, "undef", 5) == 0) {
				buf[strlen(buf) - 1] = '\0';
				delete_entry(map, buf + 7);
			}
		} else {
			if (strlen(buf) == 1 && buf[0] == '\n')
				;
			else {
				int *v;
				int n;

				get_apostrophes(buf, &v, &n);
				int j;

				for (j = 0; j < (int)map->capacity; j++)
					if (map->entries[j].key != NULL) {
						if (strstr(
							buf,
							map->entries[j].key)) {
							int pos =
							    strstr(
								buf,
								map->entries[j]
								    .key) -
							    buf;

							if (check_not_in_between(
								pos, n, v)) {

								char *aux;

								aux = (char *)
								    malloc(
									256 *
									sizeof(
									    char));
								strcpy(aux,
								       buf);
								strncpy(buf,
									aux,
									pos);
								strcpy(
								    buf + pos,
								    map
									->entries
									    [j]
									.value);
								strcpy(
								    buf + pos +
									strlen(
									    map
										->entries
										    [j]
										.value),
								    aux + pos +
									strlen(
									    map
										->entries
										    [j]
										.key));
								free(aux);
							}
							while (strstr(
							    buf + pos +
								strlen(
								    map
									->entries
									    [j]
									.key),
							    map->entries[j]
								.key)) {
								pos =
								    strstr(
									buf +
									    pos +
									    strlen(
										map
										    ->entries
											[j]
										    .key),
									map
									    ->entries
										[j]
									    .key) -
								    buf;
								if (check_not_in_between(
									pos, n,
									v)) {

									char *
									    aux;

									aux = (char *)malloc(
									    256 *
									    sizeof(
										char));
									strcpy(
									    aux,
									    buf);
									strncpy(
									    buf,
									    aux,
									    pos);
									strcpy(
									    buf +
										pos,
									    map
										->entries
										    [j]
										.value);
									strcpy(
									    buf +
										pos +
										strlen(
										    map
											->entries
											    [j]
											.value),
									    aux +
										pos +
										strlen(
										    map
											->entries
											    [j]
											.key));
									free(
									    aux);
								}
							}
						}
					}

				fprintf(outfd, "%s", buf);
				if (n)
					free(v);
			}
		}
	}

	if (infile != NULL) {
		free(infile);
		fclose(infd);
	}
	if (outfile != NULL) {
		free(outfile);
		fclose(outfd);
	}

	ht_destroy(map);

	return 0;
}
