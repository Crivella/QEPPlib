#ifndef QEPP_READ_H
#define QEPP_READ_H

#define _GNU_SOURCE

#include <stdio.h>		//fopen(), fprintf(), printf(), scanf()
#include <stdlib.h>		//malloc(), realloc()
#include <string.h>		//strstr(), strcpy(), strlen()
#include <math.h>		//pow()
#include <stdbool.h>
#include <assert.h>

#include <qepp/my_lapack.h>	//delta_k(),
#include <qepp/qepp_err.h>	//errh, SET_ERRH(), WARN(), FAIL()
#include <qepp/qepp_io.h>	//QEPP_PRINT(), QEPP_ALL_PRINT(), QEPP_OUT()
#include <qepp/qepp_file.h>
#include <qepp/qepp_struct.h>

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
		pseudo **:	parse_errh(read_pseudo(     (char *)a, (pseudo **)b)), \
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
			pseudo **:	parse_errh(read_pseudo(     (char *)b, (pseudo **)a)), \
			default:	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
		), \
		default: 	parse_errh( set_errh( WARNING, __func__, "Calling macro READ type not implemented...\n")) \
	)

#define dump_size 12

//__attribute__ ((visibility ("default")))

errh * read_nscf_md(    const char *, nscf_md **);
errh * read_nscf_data(  const char *, nscf_data **);
errh * read_band_data(  const char *, band_data **);
errh * read_band_pp(    const char *, band_pp **);
errh * read_spin_data(  const char *, spin_data **);
errh * read_opt_data(   const char *, opt_data **, char *);
errh * read_m_elem(     const char *, m_elem **, char *);
errh * read_fit_params( const char *, fit_params **);
errh * read_data_set(   const char *, data_set **);
errh * read_pdos_data(  const char *, pdos_data **, char *);
errh * read_pdos_state( const char *, pdos_state **);

errh * read_pseudo(     const char *, pseudo **);

errh * read_wfc( const char * filename, wfc **);
errh * read_data_file( const char * filename, data_file **);

errh * read_wfc_xml( const char * filename, wfc **);

errh * read_wfc_dat( const char * filename, wfc **);

errh * read_pseudo( const char * filename, pseudo **);










#endif //QEPP_READ_H










