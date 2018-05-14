#ifndef WRAPPER_LIBXML_H
#define WRAPPER_LIBXML_H

#ifdef __LIBXML2

#include <string.h>		//strlen(), strcpy()
#include <stdlib.h>		//malloc()
#include <libxml/parser.h>	//xmlDocPtr, xmlNodePtr, xmlStrcml()

xmlNodePtr	qepp_libxml_find_node( char * name, xmlNodePtr node);
//xmlChar ** 	qepp_libxml_get_attr( xmlNodePtr node);





#endif //__LIBXML2

#endif //WRAPPER_LIBXML_H








