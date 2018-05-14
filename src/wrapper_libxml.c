#include <qepp/wrapper_libxml.h>


xmlNodePtr qepp_libxml_find_node( char * name, xmlNodePtr root)
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
				res = qepp_libxml_find_node( ptr+strlen(ptr)+1, node);
			else
			{
				res = node;
				break;
			}
		}
	}	

	return res;
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















