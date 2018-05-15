#include <qepp/qepp_file.h>

//static void free_str_array( char ** ptr);
//static char ** get_dir_content( char * dirpath);
static long int print_str_array_num( char ** ptr, char * outname);
static char	* get_path( char *);

char * qepp_get_file( char * filename, char * id)
{
	char * endres=NULL;
	if( qepp_is_file( filename))
	{
		endres = malloc( (strlen( filename)+1) * sizeof( char));
		strcpy( endres, filename);
		return endres;
	}
	char ** app = get_dir_content( NULL);
	char ** res=NULL;
	char * ptr;
	int n=0;
	for( int i=0; (ptr = app[i]) != NULL; i++)
	{
		//printf("tocomp: %s\n",ptr);
		if( strstr( ptr, id) != NULL)
		{
			//printf("Ok with n: %d\n",n);
			//int check = 0;
			//if( type == NULL)
			//	check = 1;
			//else if( strstr( ptr, type) != NULL)
			//	check = 1;
			//if( check)
			//{
				res = realloc( res, (n+1) * sizeof( char *));
				int len = strlen( ptr);
				res[n] = malloc( (len+1) * sizeof( char));
				strcpy( res[n], ptr);
				n++;
			//}
		}
	}
	res = realloc( res, (n+1) * sizeof( char *));
	res[n] = 0;

	if( res[0] == NULL)
		fprintf( stderr, "WARNING qepp_get_file(): No valid output found...\n");
	else
	{
		printf("Choose the nscf output among the list:\n");
		long int max_sel = print_str_array_num( res, 0);
		long int sel=-1;
		while( sel<1 || sel >max_sel)
		{
			if( scanf("%li",&sel)) {}
			if(sel<1 || sel >max_sel)
				fprintf( stderr, "Selection out of range...\n");
		}
		endres = malloc( (strlen( res[sel-1])+1) * sizeof( char));
		strcpy( endres, res[sel-1]);
	}

	free_str_array( app);
	free_str_array( res);

	return endres;
}

static char	* get_path( char * str)
{
	char * res = NULL;
	if( str == NULL)
		fprintf( stderr, "get_path:\tPassing null pointer\n");
	else
	{
		int len = strlen( str);
		res = malloc( (len+1) * sizeof( char));
		strcpy( res, str);
		for( int i=len; res[i] != '/' && i>=0; i--)
			res[i] = '\0';
	}
	return res;
}

char 	* qepp_change_file( char * old, char * new)
{
	char * res = NULL;
	if( old == NULL)
		fprintf( stderr, "qepp_change_file:\tPassing null pointer\n");
	else if( new == NULL)
		fprintf( stderr, "qepp_change_file:\tPassing null pointer\n");
	else
	{
		char * app = get_path( old);
		int len1 = strlen( old);
		int len2 = strlen( new);

		res = malloc( (len1+len2+1) * sizeof( char));
		sprintf( res, "%s%s", app, new);
		free( app);
	}

	return res;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Read a line from file into the buffer (NULL terminated, include the \n)
size_t qepp_getline(char buffer[], int max_size, FILE * read)
{
	size_t numchars=0;
	char c;
	do
	{
		c=fgetc(read);
		buffer[numchars++]=c;
	} while(c != '\n' && !feof(read) && numchars < max_size);
	if( numchars == 1 && feof(read))
		numchars--;
	buffer[numchars]=0;
	c=fgetc(read);
	if(!feof(read))
		fseek(read,-1,SEEK_CUR);

	return numchars;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long int qepp_skip_comments( FILE * read, char * comments)
{
	long int nl=0;
	long int pos=0;

	char buffer[256];
	do {
		pos = ftell( read);
		qepp_getline( buffer, 256, read);
		nl++;
	} while( !feof( read) && strchr( comments, buffer[0]) != NULL);
	fseek( read, pos, SEEK_SET);

	return nl;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get numb of lines in file (exclude comments)
long int qepp_get_file_lines_comm(FILE * read, char * comments)
{
	long int pos = ftell(read);
	long int lines=0;
	char buffer[256];
	rewind(read);
	while(!feof(read))
	{
		qepp_getline(buffer,256,read);
		if(comments != NULL)
		{
			if(strchr(comments,buffer[0]) == NULL) //If line is not a comment
				lines++;
		}
		else
			lines++;
	}

	fseek(read, pos, SEEK_SET); 
	return lines;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get numb of columns in file
int qepp_get_file_columns_comm(FILE * read, char * comments, char * delimiters)
{
	int col=0;
	char buffer[256];
	char delim[64];
	char * default_delim=" \t";
	long int pos = ftell(read);
	int flag_num=0;
	if(delimiters == NULL)
		strcpy(delim,default_delim);
	else
		strcpy(delim,delimiters);

	rewind(read);
	qepp_getline(buffer,256,read);
	if(comments != NULL)
	{
		while( strchr(comments,buffer[0]) != NULL && !feof(read))
			qepp_getline(buffer,256,read);
	}
	for( int i=0; buffer[i] != 0; i++)
	{
		if( strchr(delim,buffer[i]) == NULL && buffer[i] != '\n')
		{
			if(flag_num==0)
			{
				flag_num=1;
				col++;
			}
		}
		else
			flag_num=0;
	}

	fseek(read, pos, SEEK_SET); 
	return col;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool qepp_strcmp_WC( char * pattern, char * candidate, int p, int c)
{
	if(pattern == NULL || candidate == NULL)
		return false;
	if (pattern[p] == '\0')
		return candidate[c] == '\0';
	else if (pattern[p] == '*')
	{
		for (; candidate[c] != '\0'; c++)
			if (qepp_strcmp_WC(pattern, candidate, p+1, c))
				return true;
   		return qepp_strcmp_WC(pattern, candidate, p+1, c);
	}
	else if ((pattern[p] != '?' && pattern[p] != candidate[c]) || candidate[c] == '\0')
    		return false;
	else
    		return qepp_strcmp_WC(pattern, candidate, p+1, c+1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void free_str_array( char ** ptr)
{
	if(ptr == NULL)
		return ;
	for( int i=0; ptr[i] != NULL; i++)
		free(ptr[i]);
	free(ptr);
	return ;
}

char ** get_dir_content( char * dirpath)
{
	char ** obj = NULL;
	if( dirpath == NULL)
		dirpath = "./";

	DIR * dir;
	struct dirent * ent;
	if ( (dir = opendir (dirpath)) == NULL)
	{
		perror("");
		return NULL;
	}
	int objects=0;
	while ((ent = readdir (dir)) != NULL)
	{
		int len = strlen(ent->d_name);
		obj = realloc( obj, (objects+1) * sizeof( char *));
		obj[objects] = calloc( len+1, sizeof( char));
		strcpy( obj[objects], ent->d_name);
		objects++;
	}
	closedir (dir);

	obj = realloc( obj, (objects+1) * sizeof( char *));
	obj[objects] = NULL;

	return obj;
}

bool qepp_is_file( char * filpath)
{
	struct stat path_stat;
	stat( filpath, &path_stat);
	if( S_ISREG(path_stat.st_mode))
		return true;
	return false;
}

bool qepp_is_dir( char * dirpath)
{
	struct stat path_stat;
	stat( dirpath, &path_stat);
	if( S_ISDIR(path_stat.st_mode))
		return true;
	return false;
}

static long int print_str_array_num( char ** string, char * outname)
{
	if( string == NULL)
		return -1;
	FILE * write = NULL;
	if( outname != NULL)
		write = fopen( outname, "w");
	if( write == NULL)
		write = stdout;

	long int i=0;
	for( ; string[i] != NULL; i++)
		//if( string[i][0] != '\0')
			//fprintf(write,"%p[%d] -> %p -> %s\n",string,i,string[i], string[i]);
			fprintf(write,"%li- %s\n", i+1, string[i]);

	if( write != stdout)
		fclose(write);

	return i;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Find substring in file and return number(in ftell) of first line where it was found (start from start_pos)
long int qepp_find_string(char * pattern, FILE * read, long int start_pos)
{
	if( read == NULL)
		return -1;
	long int line = -1;
	long int pos = ftell(read);
	
	if(start_pos >= 0)
		fseek(read, start_pos, SEEK_SET);
	char buffer[256];
	while(!feof(read))
	{
		line=ftell(read);
		qepp_getline(buffer,256,read);
		if( strstr(buffer,pattern) != NULL)
			break;
	}
	if(feof(read))
		line=-1;

	fseek(read, pos, SEEK_SET);
	return line;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get associated value
int qepp_get_value( char * pattern, FILE * read, char * delim, long int start_pos, enum gvk gvk_e, void * res)
{
	long int pos = ftell(read);

	char buffer_1[256];
	long int app = qepp_find_string( pattern, read, start_pos);
	if(app == -1)
		return 1;
	fseek(read,app,SEEK_SET);
	qepp_getline(buffer_1,256,read);
//printf( "%s\n",buffer_1);

	int check = 0;
	if( delim != NULL)
	{
		for( int i=0; delim[i] != '\0'; i++)
			if( strchr(buffer_1, delim[i]) != NULL)
				check = 1;
		if( !check)
			return 1;
	}
		
	check = 0;
	if( delim != NULL)
		if( strchr(delim, buffer_1[0]) != NULL)
			check = 1; 

	char * tokens[16];
	if( delim != NULL)
	{
		tokens[0] = strtok( buffer_1, delim);
		int count=0;
		while(tokens[count] != NULL)
			tokens[++count] = strtok( NULL, delim);
	}
	else
		tokens[1] = buffer_1;

	void * ptr;

	if( !check)
		ptr = tokens[1];
	else
		ptr = tokens[0];

//printf("%s\n",ptr);

	fseek(read, pos, SEEK_SET);

	int result=0;
	char * result_str = (char *)1;

	switch(gvk_e)
	{
	case R_INT: result = sscanf( ptr, "%d", (int *)res);			break;
	case R_FLT: result = qepp_sscanf_double( ptr, (double *)res, NULL);	break;
	case R_STR: result_str = strcpy( res, ptr);				break;
	case R_LNT: result = sscanf( ptr, "%li", (long int *)res);		break;
	case R_LNF: result = sscanf( ptr, "%lf", (double *)res);		break;
	}

//printf("%d  %li  %lf\n",*(int *)res,*(long int *)res,*(double *)res); 

	if( result == EOF || result_str == NULL)
		return 1;

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Get double from file without using fscanf
int qepp_fscanf_double(FILE * read, double * res)
{
	if( read == NULL)
	{
		fprintf(stderr,"ERROR FUNCTION my_scanf_double:\tPassing NULL file pointer.\n");
		return EOF;

	}
	char c='\0';
	while( (c < '0' || c > '9') && c != EOF )
	{
		c=getc(read);
		if( c == '.' || c == '-' || c == '+')
		{
			char app=c;
			c=getc(read);
			if( ('0' <= c && c <= '9'))
			{
				c=app;
				fseek(read,-1,SEEK_CUR);
				break;
			}
			fseek(read,-1,SEEK_CUR);
		}
	}
	if( c == '\0' || c == EOF)
		return EOF;

	*res = 0;
	int negative=0, fase=0;
	int decimal=1, exponent=0;
	int forcequit=0;
	while( c != ' ' && c != '\t' && c != '\n' && c != EOF && !forcequit)
	{
		switch(c)
		{
		case '-':
			if(fase==0 && *res==0)
				negative=1;
			else 
				if(fase == 2 && exponent==0)
					fase=3;
				else
				{
					fseek(read,-1,SEEK_CUR);
					forcequit=1;
				}
			
			break;
		case '.':
			fase=1;
			break;
		case 'e':
		case 'd':
		case 'E':
			fase=2;
			break;

		default:
			if( (c < '0' || c > '9') && c != '+')
			{
				forcequit = 1;
				break;
			}
			switch(fase)
			{
			case 0:	//integer part
				*res *= 10;
				*res += (c-'0');
				break;
			case 1:	//decimal part
				*res += (double)((c-'0')*pow(10,-decimal));
				decimal++;
				break;
			case 2:	//positive exponent
				if( c == '+')
					break;
				exponent *= 10;
				exponent += (int)(c-'0');
				break;
			case 3:	//negative exponent
				exponent *= 10;
				exponent -= (int)(c-'0');
				break;
			}
			break;
		}
		c=getc(read);
	}
	if(negative == 1)
		*res*=(double)(-1);
	*res*=pow( 10, exponent);
	if(!feof(read))
		fseek(read,-1,SEEK_CUR);

	return 0;
}

int qepp_sscanf_double2(char * str, double * res , char ** endptr)
{
	if( str == NULL)
	{
		fprintf(stderr,"ERROR FUNCTION my_scanf_double:\tPassing NULL char * pointer.\n");
		return EOF;
	}
	int count = -1;
	char c=(char)1;
	while( (c < '0' || c > '9') && c != '\0' )
	{
		c=str[++count];
		if( (c == '.' || c == '-' || c == '+') && ( '0' <= str[count+1] && str[count+1] <= '9'))
			break;
	}
	if( c == '\0')
		return 1;
	*res = 0;
	int negative=0, fase=0;
	int decimal=1, exponent=0;
	int forcequit=0;
	while( c != ' ' && c != '\t' && c != '\n' && c != '\0' && !forcequit)
	{
		switch(c)
		{
		case '-':
			if(fase==0 && *res==0)
				negative=1;
			else 
				if(fase == 2 && exponent==0)
					fase=3;
				else
				{
					count--;
					forcequit=1;
				}
			
			break;
		case '.':
			fase=1;
			break;
		case 'e':
		case 'd':
		case 'E':
			fase=2;
			break;

		default:
			if( (c < '0' || c > '9') && c != '+')
			{
				count--;
				forcequit = 1;
				break;
			}
			switch(fase)
			{
			case 0:	//integer part
				*res *= 10;
				*res += (c-'0');
				break;
			case 1:	//decimal part
				*res += (double)((c-'0')*pow(10,-decimal));
				decimal++;
				break;
			case 2:	//positive exponent
				if( c == '+')
					break;
				exponent *= 10;
				exponent += (int)(c-'0');
				break;
			case 3:	//negative exponent
				exponent *= 10;
				exponent -= (int)(c-'0');
				break;
			}
			break;
		}
		c=str[++count];
	}
	if(negative == 1)
		*res*=(double)(-1);
	*res*=pow( 10, exponent);
	if( endptr != NULL)
		*endptr = str+count;

	return 0;
}

int qepp_get_dat_attr( double * out_ptr, FILE * read, long int pos, char * name, char * key)
{
	if( out_ptr == NULL)
		return 1;
	char * ptr;
	double res;
	char buffer[1024]="void";
	char needle[128];

	sprintf( needle, "<%s ", name);

	fseek( read, pos, SEEK_SET);
	while( strstr( buffer, needle)==NULL && !feof( read))
		qepp_getline( buffer, 1024, read);
	if( feof( read))
		return 1;

	ptr = strstr( buffer, key);
	if( ptr != NULL)
		qepp_sscanf_double( ptr, &res, &ptr);

	*out_ptr = res;
	return 0;
}

int qepp_get_dat_value( void * out_ptr, FILE * read, long int pos, char * name, long int num, int size, int dump_s)
{
	if( out_ptr == NULL)
		return 1;
	char buffer[1024]="void";
	char needle[128];
	sprintf( needle, "<%s ", name);
	void * dump = malloc( dump_s);

	fseek( read, pos, SEEK_SET);
	while( strstr( buffer, needle)==NULL && !feof( read))
		qepp_getline( buffer, 1024, read);
	if( feof( read))
		return 1;

	if( dump_s > 0)
		if( fread( dump, dump_s, 1, read) < 1)
			return 1;

	if( fread( out_ptr, size, num, read) < num)
		return 1;

	free( dump);
	return 0;
}

char * get_tmp_path()
{
	char * res = NULL;

	char ** app1, ** app2;
	app1 = get_dir_content( NULL);
	for( int i=0; app1[i]!=NULL; i++)
	{
		if( strcmp( app1[i], "tmp") == 0)
		{
			app2 = get_dir_content( app1[i]);
			for( int j=0; app2[j]!=NULL; j++)
			{
				if( qepp_strcmp_wc( "*.save", app2[j]))
				{
					res = malloc( 1024);
					sprintf( res, "./%s/%s", app1[i], app2[j]);
					free_str_array( app1);
					free_str_array( app2);
					return res;
				}
			}
			free_str_array( app1);
			free_str_array( app2);
			return NULL;
		}
	}
	free_str_array( app1);

	return NULL;
}


int qepp_get_str( char * ptr, char *res)
{
	if( ptr == NULL)
		return 1;
	if( res == NULL)
		return 1;
	char buff[1024];
	char c;
	int c_1=0, c_2=0;
	int check=0;
	do
	{
		c = ptr[c_1++];
		if( c == '\'' || c == '\"')
		{
			check++;
			c = ptr[c_1++];
		}
		if( check == 1)
		{
			if( !isspace( c))
				buff[c_2++] = c;
		}
	} while (c != '\0' && c != '\n' && check <=2);

	buff[c_2] = '\0';
	if( check != 2)
		return 1;
	strcpy( res, buff);

	return 0;
}

void qepp_trim_ws( char * ptr)
{
	if( ptr == NULL)
		return;

	char buff[1024];
	char c;
	int c_1=0, c_2=0;
	do
	{
		c = ptr[c_1++];
		if( !isspace( c) || c == '\n')
			buff[c_2++] = c;
	} while (c != '\0');

	strcpy( ptr, buff);

	return ;
}
















