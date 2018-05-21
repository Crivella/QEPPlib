#ifndef WRAPPER_LIBXML_H
#define WRAPPER_LIBXML_H

#ifdef __LIBXML2

#include <string.h>		//strlen(), strcpy()
#include <stdlib.h>		//malloc()
#include <qepp/qepp_file.h>	//enum gvk, qepp_sscanf_double()
#include <libxml/parser.h>	//xmlDocPtr, xmlNodePtr, xmlStrcml()

#define SECOND(a, b, ...) b
#define qepp_libxml_find_node( a, b, ...) qepp_libxml_find_node2( a, b, SECOND( 0, ##__VA_ARGS__, 0))
xmlNodePtr	qepp_libxml_find_node2( char * name, xmlNodePtr node, unsigned long int skip);
int		qepp_libxml_get_node_value( void * res, xmlNodePtr node, enum gvk type, unsigned long int num);
int		qepp_libxml_get_node_attr( void * res, char * name, xmlNodePtr node, enum gvk type, unsigned long int num);
//xmlChar ** 	qepp_libxml_get_attr( xmlNodePtr node);





#endif //__LIBXML2

#endif //WRAPPER_LIBXML_H








