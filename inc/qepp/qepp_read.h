#ifndef QEPP_READ_H
#define QEPP_READ_H

#define _GNU_SOURCE

#include <stdio.h>	//fopen(), fprintf(), printf(), scanf()
#include <stdlib.h>	//malloc(), realloc()
#include <string.h>	//strstr(), strcpy(), strlen()
#include <stdbool.h>

#include <qepp/my_lapack.h>
#include <qepp/qepp_err.h>
#include <qepp/qepp_io.h>
#include <qepp/qepp_file.h>
#include <qepp/qepp_struct.h>

#ifdef __LIBXML2
#include <libxml/parser.h>
#endif



#define SECOND(a, b, ...) b

#define READ( a, b, ...) \
	_Generic( (b), \
		nscf_data **:	parse_errh(read_nscf_data(  (char *)a, (nscf_data **)b)), \
		band_data **:	parse_errh(read_band_data(  (char *)a, (band_data **)b)), \
		spin_data **:	parse_errh(read_spin_data(  (char *)a, (spin_data **)b)), \
		band_pp **:	parse_errh(read_band_pp(    (char *)a, (band_pp **)b)), \
		nscf_md **:	parse_errh(read_nscf_md(    (char *)a, (nscf_md **)b)), \
		opt_data **:	parse_errh(read_opt_data(   (char *)a, (opt_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		m_elem **:	parse_errh(read_m_elem(     (char *)a, (m_elem **)b, SECOND( 0, ##__VA_ARGS__, NULL))),\
		fit_params **:	parse_errh(read_fit_params( (char *)a, (fit_params **)b)), \
		data_set **:	parse_errh(read_data_set(   (char *)a, (data_set **)b)), \
		pdos_data **:	parse_errh(read_pdos_data(  (char *)a, (pdos_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
		pdos_state **:	parse_errh(read_pdos_state( (char *)a, (pdos_state **)b)), \
		data_file **:	parse_errh(read_data_file(  (char *)a, (data_file **)b)), \
		wfc **:		parse_errh(read_wfc(        (char *)a, (wfc **)b)), \
		gkv **:		parse_errh(read_gkv(        (char *)a, (gkv **)b)), \
		egv **:		parse_errh(read_egv(        (char *)a, (egv **)b)), \
		char *:		_Generic( (a), \
			nscf_data **:	parse_errh(read_nscf_data(  (char *)b, (nscf_data **)a)), \
			band_data **:	parse_errh(read_band_data(  (char *)b, (band_data **)a)), \
			spin_data **:	parse_errh(read_spin_data(  (char *)b, (spin_data **)a)), \
			band_pp **:	parse_errh(read_band_pp(    (char *)b, (band_pp **)a)), \
			nscf_md **:	parse_errh(read_nscf_md(    (char *)b, (nscf_md **)a)), \
			opt_data **:	parse_errh(read_opt_data(   (char *)b, (opt_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
			m_elem **:	parse_errh(read_m_elem(     (char *)b, (m_elem **)b, SECOND( 0, ##__VA_ARGS__, NULL))),\
			fit_params **:	parse_errh(read_fit_params( (char *)b, (fit_params **)a)), \
			data_set **:	parse_errh(read_data_set(   (char *)b, (data_set **)a)), \
			pdos_data **:	parse_errh(read_pdos_data(  (char *)b, (pdos_data **)b, SECOND( 0, ##__VA_ARGS__, NULL))), \
			pdos_state **:	parse_errh(read_pdos_state( (char *)b, (pdos_state **)a)), \
			data_file **:	parse_errh(read_data_file(  (char *)b, (data_file **)a)), \
			wfc **:		parse_errh(read_wfc(        (char *)b, (wfc **)a)), \
			gkv **:		parse_errh(read_gkv(        (char *)b, (gkv **)a)), \
			egv **:		parse_errh(read_egv(        (char *)b, (egv **)a)), \
			default:	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
		), \
		default: 	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
	)

#define dump_size 12

errh * read_nscf_md(char *, nscf_md **);
errh * read_nscf_data(char *, nscf_data **);
errh * read_band_data(char *, band_data **);
errh * read_band_pp(char *, band_pp **);
errh * read_spin_data(char *, spin_data **);
errh * read_opt_data(char *, opt_data **, char *);
errh * read_m_elem(char *, m_elem **, char *);
errh * read_fit_params(char *, fit_params **);
errh * read_data_set(char *, data_set **);
errh * read_pdos_data(char *, pdos_data **, char *);
errh * read_pdos_state(char *, pdos_state **);

errh * read_wfc( const char * filename, wfc **);
errh * read_gkv( const char * filename, gkv **);
errh * read_egv( const char * filename, egv **);
#ifdef __LIBXML2
errh * read_data_file( const char * filename, data_file **);
errh * read_wfc_xml( const char * filename, wfc **);
evc  * read_evc_xml( xmlDoc * document, xmlNode * root, xmlNode * node);
errh * read_gkv_xml( const char * filename, gkv **);
errh * read_egv_xml( const char * filename, egv **);
#endif //__LIBXML2
errh * read_wfc_dat( const char * filename, wfc **);
evc  * read_evc_dat( FILE *, int);
errh * read_gkv_dat( const char * filename, gkv **);










#endif //QEPP_READ_H










