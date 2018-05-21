#include <qepp/wrapper_libxml.h>

#ifdef __LIBXML2
xmlNodePtr qepp_libxml_find_node( char * name, xmlNodePtr root, unsigned long int skip)
{
	if( name == NULL)
		return NULL;
	else if( name[0] == '\0')
		return NULL;

	xmlNodePtr	node, res = NULL;

	if( root == NULL)
		return NULL;

	char buffer[1024];
	strcpy( buffer, name);
	char * ptr = strtok( buffer, "/");

	for( node = root->children; node; node=node->next)
	{
		if( !xmlStrcmp( node->name, (const xmlChar *)ptr))
		{
			if( strcmp( ptr, name))
			{
				res = qepp_libxml_find_node( ptr+strlen(ptr)+1, node, skip);
			}
			else
			{
				if( skip > 0)
					skip--;
				else
				{
					res = node;
					break;
				}
			}
		}
	}	

	return res;
}

int qepp_libxml_get_node_value( void * res, xmlNodePtr node, enum gvk type, unsigned long int num)
{
	if( node == NULL)
		return 1;

	xmlChar * key;
	xmlDocPtr document;

	document = node->doc;
	if( document == NULL)
		return 1;

	key = xmlNodeListGetString( document, node->xmlChildrenNode, 1);
	char * endptr = (char *)key;

	for( unsigned long int i=0; i<num; i++)
	{
		switch( type)
		{
			case R_INT:
				//sscanf( endptr, "%d", (int *)res+i*sizeof( int));
				*((int *)res+i) = strtol( endptr, &endptr, 10);
				break;
			case R_LNT:
				*((long int *)res+i) = strtol( endptr, &endptr, 10);
				break;
			case R_FLT:
				*((float *)res+i) = strtof( endptr, &endptr);
				//sscanf( endptr, "%f", (float *)res+i*sizeof( float));
				break;
			case R_LNF:
				assert( !qepp_sscanf_double( endptr, res+i*sizeof(double), &endptr));
				break;
			case R_STR:
				if( num != 1)
					return 1;
				strcpy( res, endptr);
				break;\
		}
	}
	xmlFree( key);

	return 0;
}

int qepp_libxml_get_node_attr( void * res, char * name, xmlNodePtr node, enum gvk type, unsigned long int num)
{
	if( node == NULL)
		return 1;

	xmlChar * key;
	xmlDocPtr document;

	document = node->doc;
	if( document == NULL)
		return 1;

	key = xmlGetProp( node, (const xmlChar *)name);
	char * endptr = (char *)key;
	for( unsigned long int i=0; i<num; i++)
	{
		switch( type)
		{
			case R_INT:
				sscanf( endptr, "%d", (int *)res+i*sizeof( int));
				break;
			case R_LNT:
				sscanf( endptr, "%li", (long int *)res+i*sizeof(long int));
				break;
			case R_FLT:
				sscanf( endptr, "%f", (float *)res+i*sizeof( float));
				break;
			case R_LNF:
				assert( !qepp_sscanf_double( endptr, res+i*sizeof(double), &endptr));
				break;
			case R_STR:
				if( num != 1)
					return 1;
				strcpy( res, endptr);
				break;
		}
	}


	return 0;
}

/*
xmlChar ** 	qepp_libxml_get_attr( xmlNodePtr node)
{
	if( node == NULL)
		return NULL;

	int count=0;
	xmlChar ** res = NULL;

	//xmlDocPtr	document = node->doc;
	xmlAttrPtr	attr;

	attr=node->properties;
	if( attr != NULL)
	{
		do{ 
			res = realloc( res, ++count * sizeof( const xmlChar *));
			res[count] = attr->name;
		} while( (attr = attr->next) != NULL) ;
		res = realloc( res, ++count * sizeof( xmlChar *));
		res[count] = NULL;
	}
	

	return res;
}
*/


#endif //__LIBXML2














