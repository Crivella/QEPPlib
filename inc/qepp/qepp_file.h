#ifndef QEPP_FILE_H
#define QEPP_FILE_H

#define _GNU_SOURCE

#include <stdio.h>	//fopen(), printf(), fprintf(), sprintf(), fclose(), getc(), sscanf()
#include <stdlib.h>	//malloc(), calloc(), realloc(), free(), realpath()
#include <string.h>	//strcmp(), strcasecmp(), strcpy(), strlen(), memcpy(), strstr(), strcasestr(), strchr(), strtok()
#include <math.h>	//pow()
#include <ctype.h>	//isspace()
#include <stdbool.h>
#include <sys/types.h>	//mode_t
#include <sys/stat.h>	//struct stat, stat(), S_ISREG()
#include <dirent.h>	//DIR, struct dirent, opendir(), readdir(), closedir()

#include <qepp/qepp_err.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Macros
#ifndef JUST_MACRO
#define JUST_MACRO
#define JUST1(a, ...) (a)
#define JUST2(a, b, ...) (a), (b)
#define JUST3(a, b, c, ...) (a), (b), (c)
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

char	* get_file(char *, char *);
char	* get_path( char *);
char 	* change_file( char *, char *);
FILE * open_qe_in( char * filename);
FILE * open_qe_out( char * filename);

size_t my_getline( char buffer[], int max_size, FILE * read);
long int get_file_size(FILE * read);
long int skip_comments( FILE * read, char * comments);
long int get_file_lines_comm(FILE * read, char * comments);
#define GET_FILE_LINES(...) get_file_lines_comm(JUST2(__VA_ARGS__,0,0))
int get_file_columns_comm(FILE * read, char * comments, char * delimiters);
#define GET_FILE_COL(...) get_file_columns_comm(JUST3(__VA_ARGS__,0,0,0))
long int * get_file_lines_pos(FILE * read);

bool strcmp_WC( char * pattern, char * candidate, int p, int c);
#define strcmp_wc(a,b,...) strcmp_WC(a,b,0,0)

void free_str_array( char ** ptr);
char ** get_all_match( char ** haystack, char * pattern, char * exclude);
char ** get_dir_content( char * dirpath);
char * get_one_match( char ** haystack, char * pattern);
bool is_file( char * filpath);
bool is_dir( char * dirpath);
char ** load_file( char * filpath);
void * print_str_array( char ** ptr, char * outname);
long int print_str_array_num( char ** ptr, char * outname);
void sort_str_array( char ** ptr);

long int find_string(char * pattern, FILE * read, long int start_pos);
enum gvk { R_INT, R_FLT, R_STR, R_LNT, R_LNF}; //get_value_kind
int get_value( char * pattern, FILE * read, char * delim, long int start_pos, enum gvk gvk_e, void * res);

int my_fscanf_double(FILE * to_read, double * res);
int my_sscanf_double(char * , double *);
int my_sscanf_double2(char * , double *, char **);

void switch_val(void * a, void * b, size_t size);
#define SWITCH(a,b) switch_val(a,b, sizeof(*a))
#define COPY(a,b) memcpy(a,b,sizeof(*a))

void strip_esc_seq( char * string);
void strip_esc_seq_lvl( void ** ptr, int lvl);

int get_str( char * ptr, char * res);
void trim_ws( char * ptr);


/*-------------------------------------------------------------------------*/
/**
  @name     get_xml_param
  @brief    Get the value of a param from an xml file

  @param    out_ptr: Address to a pointer where the data will be stored
  @param    read: FILE handle for the file to be read
  @param    pos: Position in the file form which to start the search
  @param    name: Name of the xml class
  @param    key: Name of the paramenter in the xml class

  Start reading the file "read" from position "pos" until it finds a class
  named "name" or it reaches the EOF. If the class is found look for the
  parameter named "key" and store its value at the memory address indicated by
  "out_ptr"

  return 0 if successfull or 1 if failed
 */
/*-------------------------------------------------------------------------*/
int get_xml_param( double * out_ptr, FILE * read, long int pos, char * name, char * key);

/*-------------------------------------------------------------------------*/
/**
  @name     get_xml_value
  @brief    Get the binary value stored in a class of an xml file

  @param    out_ptr: Address to a pointer where the data will be stored
  @param    read: FILE handle for the file to be read
  @param    pos: Position in the file form which to start the search
  @param    name: Name of the xml class
  @param    num: Number of elements to be read
  @param    size: size in bytes of each element
  @param    dump_s: Size of non-useful data at the beginning of the binary data

  Start reading the file "read" from position "pos" until it finds a class
  named "name" or it reaches the EOF. If the class is found discard the first
  "dump_s" bytes of data and read "num*size" bytes of data storing them at
  the address indicated by "out_ptr" (the necessary memory for the data is 
  allocated within the function)

  return 0 if successfull or 1 if failed
 */
/*-------------------------------------------------------------------------*/
int get_xml_value( void ** out_ptr, FILE * read, long int pos, char * name, long int num, int size, int dump_s);

char * get_tmp_path();






#endif














