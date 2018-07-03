from .loader import *

__all__ = [ "READ", "PRINT", "DUPLICATE", "FREE", "nscf_md_ptr", "nscf_data_ptr", "opt_data_ptr", "m_elem_ptr",
            "pdos_state_ptr", "pdos_data_ptr", "data_file_ptr", "wfc_ptr", "cd_ptr", "pseudo_ptr" ]
	
####################################################################################################
# 'nscf_md' structure
class _nscf_md(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('lines',      c_long),

		     ('formula',    c_char * 128),
		     ('n_kpt',      c_long),
		     ('e_fermi',    c_double),
		     ('vc',         c_double),
		     ('n_el',       c_double),
		     ('n_bnd',      c_int),
		     ('spin_orbit', c_int),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
nscf_md_ptr = POINTER( _nscf_md)
#initialize_nscf_md          = qepp.initialize_nscf_md
#initialize_nscf_md.argtypes = [ ]
#initialize_nscf_md.restype  = nscf_md_ptr
_print_nscf_md               = qepp.print_nscf_md
_print_nscf_md.argtypes      = [ nscf_md_ptr, FILE_ptr]
_duplicate_nscf_md           = qepp.duplicate_nscf_md
_duplicate_nscf_md.argtypes  = [ nscf_md_ptr]
_duplicate_nscf_md.restype   = nscf_md_ptr
_free_nscf_md                = qepp.free_nscf_md
_free_nscf_md.argtype        = [ nscf_md_ptr]
_read_nscf_md                = qepp.read_nscf_md
_read_nscf_md.argtypes       = [ c_char_p, POINTER( nscf_md_ptr)]
_read_nscf_md.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'nscf_data' structure
class _nscf_data(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('md',         nscf_md_ptr),
		     ('n_kpt',      c_long),
		     ('n_bnd',      c_int),

		     ('kpt',        POINTER( POINTER( c_double))),
		     ('energies',   POINTER( POINTER( c_double))),
		     ('occup',      POINTER( POINTER( c_double))),
		     ('weight',     POINTER( c_double)),
		     ('tot_weight', c_double),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
nscf_data_ptr                 = POINTER( _nscf_data)
#initialize_nscf_data          = qepp.initialize_nscf_data
#initialize_nscf_data.argtypes = [ c_long, c_int]
#initialize_nscf_data.restype  = nscf_data_ptr
_print_nscf_data               = qepp.print_nscf_data
_print_nscf_data.argtypes      = [ nscf_data_ptr, FILE_ptr]
_duplicate_nscf_data           = qepp.duplicate_nscf_data
_duplicate_nscf_data.argtypes  = [ nscf_data_ptr]
_duplicate_nscf_data.restype   = nscf_data_ptr
_free_nscf_data                = qepp.free_nscf_data
_free_nscf_data.argtype        = [ nscf_data_ptr]
_read_nscf_data                = qepp.read_nscf_data
_read_nscf_data.argtypes       = [ c_char_p, POINTER( nscf_data_ptr)]
_read_nscf_data.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'opt_data' structure
class _opt_data(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('n_col',      c_int),
		     ('n_pt',       c_long),

		     ('x',          POINTER( c_double)),
		     ('values',     POINTER( POINTER( c_double))),
		     ('headers',    POINTER( c_char_p)),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
opt_data_ptr                 = POINTER( _opt_data)
#initialize_opt_data          = qepp.initialize_opt_data
#initialize_opt_data.argtypes = [ c_long, c_int]
#initialize_opt_data.restype  = opt_data_ptr
_print_opt_data               = qepp.print_opt_data
_print_opt_data.argtypes      = [ opt_data_ptr, FILE_ptr]
_duplicate_opt_data           = qepp.duplicate_opt_data
_duplicate_opt_data.argtypes  = [ opt_data_ptr]
_duplicate_opt_data.restype   = opt_data_ptr
_free_opt_data                = qepp.free_opt_data
_free_opt_data.argtype        = [ opt_data_ptr]
_read_opt_data                = qepp.read_opt_data
_read_opt_data.argtypes       = [ c_char_p, POINTER( opt_data_ptr), c_char_p]
_read_opt_data.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'm_elem' structure
class _m_elem(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('n_kpt',      c_long),
		     ('n_bnd',      c_int),

		     ('pp',         POINTER( POINTER( POINTER( POINTER( c_double))))),
		     ('gap',        POINTER( POINTER( POINTER( c_double)))),
		     ('df',         POINTER( POINTER( POINTER( c_double)))),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
m_elem_ptr                 = POINTER( _m_elem)
#initialize_m_elem          = qepp.initialize_m_elem
#initialize_m_elem.argtypes = [ c_long, c_int]
#initialize_m_elem.restype  = m_elem_ptr
_print_m_elem               = qepp.print_m_elem
_print_m_elem.argtypes      = [ m_elem_ptr, FILE_ptr]
_duplicate_m_elem           = qepp.duplicate_m_elem
_duplicate_m_elem.argtypes  = [ m_elem_ptr]
_duplicate_m_elem.restype   = m_elem_ptr
_free_m_elem                = qepp.free_m_elem
_free_m_elem.argtype        = [ m_elem_ptr]
_read_m_elem                = qepp.read_m_elem
_read_m_elem.argtypes       = [ c_char_p, POINTER( m_elem_ptr)]
_read_m_elem.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'pdos_state' structure
class _pdos_state(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('name',       c_char * 64),
		     ('atom_n',     c_int),
		     ('wfc_n',      c_int),

		     ('j',          c_double),
		     ('m_j',        c_double),

		     ('l',          c_int),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
pdos_state_ptr                 = POINTER( _pdos_state)
#initialize_pdos_state          = qepp.initialize_pdos_state
#initialize_pdos_state.argtypes = [ ]
#initialize_pdos_state.restype  = pdos_state_ptr
_print_pdos_state               = qepp.print_pdos_state
_print_pdos_state.argtypes      = [ pdos_state_ptr, FILE_ptr]
_duplicate_pdos_state           = qepp.duplicate_pdos_state
_duplicate_pdos_state.argtypes  = [ pdos_state_ptr]
_duplicate_pdos_state.restype   = pdos_state_ptr
_free_pdos_state                = qepp.free_pdos_state
_free_pdos_state.argtype        = [ pdos_state_ptr]
_read_pdos_state                = qepp.read_pdos_state
_read_pdos_state.argtypes       = [ c_char_p, POINTER( pdos_state_ptr)]
_read_pdos_state.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'pdos_data' structure
class _pdos_data(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('n_kpt',      c_long),
		     ('n_bnd',      c_int),
		     ('n_states',   c_int),

		     ('pdos',       POINTER( POINTER( POINTER( c_double)))),

		     ('nscf',       nscf_data_ptr),
		     ('pdos_state', POINTER( pdos_state_ptr)),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
pdos_data_ptr                 = POINTER( _pdos_data)
#initialize_pdos_data          = qepp.initialize_pdos_data
#initialize_pdos_data.argtypes = [ c_long, c_int, c_int]
#initialize_pdos_data.restype  = pdos_data_ptr
_print_pdos_data               = qepp.print_pdos_data
_print_pdos_data.argtypes      = [ pdos_data_ptr, FILE_ptr]
_duplicate_pdos_data           = qepp.duplicate_pdos_data
_duplicate_pdos_data.argtypes  = [ pdos_data_ptr]
_duplicate_pdos_data.restype   = pdos_data_ptr
_free_pdos_data                = qepp.free_pdos_data
_free_pdos_data.argtype        = [ pdos_data_ptr]
_read_pdos_data                = qepp.read_pdos_data
_read_pdos_data.argtypes       = [ c_char_p, POINTER( pdos_data_ptr)]
_read_pdos_data.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'data_file' structure
class _data_file(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('version',    c_long),

		     ('n_el',       c_double),
		     ('n_spin',     c_int),
		     ('n_bnd',      c_int),

		     ('bravais',    c_char * 64),
		     ('lp_t',       c_double),
		     ('lp',         c_double),
		     ('c_dim',      c_double * 6),

		     ('a_n',        c_char * 64),
		     ('a_t',        c_double),
		     ('a',          ( c_double * 3) * 3),

		     ('b_n',        c_char * 64),
		     ('b_t',        c_double),
		     ('b',          ( c_double * 3) * 3),

		     ('n_kpt',      c_long),
		     ('kpt_t',      c_double),
		     ('kpt',        POINTER( POINTER( c_double))),
		     ('weight',     POINTER( c_double)),

		     ('e_t',        c_double),
		     ('egv',        POINTER( POINTER( c_double))),
		     ('occ',        POINTER( POINTER( c_double))),

		     ('wfc_link',   POINTER( c_char_p)),
		     ('wfc_link2',  POINTER( c_char_p)),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
data_file_ptr                 = POINTER( _data_file)
#initialize_data_file          = qepp.initialize_data_file
#initialize_data_file.argtypes = [ ]
#initialize_data_file.restype  = data_file_ptr
_print_data_file               = qepp.print_data_file
_print_data_file.argtypes      = [ data_file_ptr, FILE_ptr]
_duplicate_data_file           = qepp.duplicate_data_file
_duplicate_data_file.argtypes  = [ data_file_ptr]
_duplicate_data_file.restype   = data_file_ptr
_free_data_file                = qepp.free_data_file
_free_data_file.argtype        = [ data_file_ptr]
_read_data_file                = qepp.read_data_file
_read_data_file.argtypes       = [ c_char_p, POINTER( data_file_ptr)]
_read_data_file.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'wfc' structure
class _wfc(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('ngw',        c_long),
		     ('igwx',       c_long),
		     ('gamma_only', c_bool),
		     ('n_bnd',      c_int),
		     ('ik',         c_long),
		     ('nk',         c_long),
		     ('i_spin',     c_int),
		     ('n_spin',     c_int),
		     ('n_bnd',      c_int),
		     ('scale_factor', c_double),

		     ('max_igwx',   c_long),
		     ('kpt',        c_double * 3),
		     ('index',      POINTER( c_int)),
		     ('grid',       POINTER( POINTER( c_int))),

		     ('val',        POINTER( POINTER( c_double * 2))),


		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
wfc_ptr                 = POINTER( _wfc)
#initialize_wfc          = qepp.initialize_wfc
#initialize_wfc.argtypes = [ ]
#initialize_wfc.restype  = wfc_ptr
_print_wfc               = qepp.print_wfc
_print_wfc.argtypes      = [ wfc_ptr, FILE_ptr]
_duplicate_wfc           = qepp.duplicate_wfc
_duplicate_wfc.argtypes  = [ wfc_ptr]
_duplicate_wfc.restype   = wfc_ptr
_free_wfc                = qepp.free_wfc
_free_wfc.argtype        = [ wfc_ptr]
_read_wfc                = qepp.read_wfc
_read_wfc.argtypes       = [ c_char_p, POINTER( wfc_ptr)]
_read_wfc.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'cd' structure
class _cd(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

		     ('x',          c_long),
		     ('y',          c_long),
		     ('z',          c_long),

		     ('coord',      POINTER( POINTER( POINTER( POINTER( c_double))))),
		     ('dens',       POINTER( POINTER( POINTER( c_double)))),

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
cd_ptr                 = POINTER( _cd)
#initialize_cd          = qepp.initialize_cd
#initialize_cd.argtypes = [ c_long, c_long, c_long]
#initialize_cd.restype  = cd_ptr
_print_cd               = qepp.print_cd
_print_cd.argtypes      = [ cd_ptr, FILE_ptr]
_duplicate_cd           = qepp.duplicate_cd
_duplicate_cd.argtypes  = [ cd_ptr]
_duplicate_cd.restype   = cd_ptr
_free_cd                = qepp.free_cd
_free_cd.argtype        = [ cd_ptr]
#read_cd                = qepp.read_cd
#read_cd.argtypes       = [ c_char_p, POINTER( cd_ptr)]
#read_cd.restype        = errh_ptr
####################################################################################################

####################################################################################################
# 'pseudo' structure
class _pseudo(Structure):
	_fields_ = [ ('typeID',     c_int),
		     ('size',       c_ulong),

	#GRID
		     ('mesh',       c_ulong),
		     ('dx',         c_double),
		     ('xmin',       c_double),
		     ('rmax',       c_double),
		     ('zmesh',      c_double),
		     ('grid',       POINTER( c_double)),		#Radial grid[1:mesh] r(i) = exmin+i*dx/Zmesh 
									#                    r(i) = (exmin+i*dx-1)/Zmesh, with r(mesh)=rmax

		     ('rab',        POINTER( c_double)),		#rab[1:mesh] factor required for discrete integration: ∫ f(r) dr =∑_i(f[i] rab[i]).

	#NLCC
		     ('rho_atc',    POINTER( c_double)),		#rho_atc(mesh) : core charge for nonlinear core correction 
									#(true charge, not multiplied by 4πr2)

	#LOCAL		
		     ('vloc',       POINTER( c_double)),		#vloc(mesh) : local potential (Ry a.u.) sampled on the radial grid
	
	#NONLOCAL	
		     ('n_beta',     c_uint),
		     ('kkbeta',     POINTER( c_ulong)),			#number of mesh points for projector i (must be ≤ mesh )
		     ('beta',       POINTER( POINTER( c_double))),	#projector r_i β(r_i) (note the factor r !!!!)
		     ('lll',        POINTER( c_uint)),			#angular momentum of projector i
		     ('cri',        POINTER( c_ulong)),			#cut radius index:  grid[i] =~ r_c
		     ('cur_rad',    POINTER( c_double)),		#cut off radius r_c of projector i
		     ('dij',        POINTER( POINTER( c_double))),	#the D_{ij} factors of the nonlocal PP: VNL = ∑_{i,j} D_{i,j} |β_i><β_j|


	#PSWFC		
		     ('natwf',      c_uint),				#number of atomic wfc
		     ('chi',        POINTER( POINTER( c_double))),	#chi(i,mesh): chi_i(r), i-th radial atomic (pseudo-)orbital
									#(radial part of the KS equation, multiplied by r)
		     ('els',        POINTER( c_uint)),			#n of thw wfc
		     ('lchi',       POINTER( c_uint)),			#angular momentum of the wfc
		     ('occ',        POINTER( c_double)),		#occupation of wfc

	#RHOATOM
		     ('rho_at',     POINTER( c_double)),		#rho_at(mesh): radial atomic (pseudo-)charge .This is 4π r2 times the true charge.

		     ('_print',     c_void_p),
		     ('_duplicate', c_void_p),
		     ('_free',      c_void_p)]
pseudo_ptr                 = POINTER( _pseudo)
#initialize_pseudo          = qepp.initialize_pseudo
#initialize_pseudo.argtypes = [ c_long, c_long, c_long]
#initialize_pseudo.restype  = pseudo_ptr
_print_pseudo               = qepp.print_pseudo
_print_pseudo.argtypes      = [ pseudo_ptr, FILE_ptr]
_duplicate_pseudo           = qepp.duplicate_pseudo
_duplicate_pseudo.argtypes  = [ pseudo_ptr]
_duplicate_pseudo.restype   = pseudo_ptr
_free_pseudo                = qepp.free_pseudo
_free_pseudo.argtype        = [ pseudo_ptr]
_read_pseudo                = qepp.read_pseudo
_read_pseudo.argtypes       = [ c_char_p, POINTER( pseudo_ptr)]
_read_pseudo.restype        = errh_ptr
####################################################################################################

#nscf_md nscf_data opt_data m_elem pdos_state pdos_data data_file wfc pseudo
switchREAD= {	nscf_md_ptr:    _read_nscf_md,
		nscf_data_ptr:  _read_nscf_data,
		opt_data_ptr:   _read_opt_data,
		m_elem_ptr:     _read_m_elem,
		pdos_state_ptr: _read_pdos_state,
		pdos_data_ptr:  _read_pdos_data,
		data_file_ptr:  _read_data_file,
		wfc_ptr:        _read_wfc,
		pseudo_ptr:     _read_pseudo   }
def READ( name, ptr, opt = None):
	if( not opt):
		app1 = switchREAD[type(ptr)]( name.encode('utf-8'), byref(ptr))
	else:
		app1 = switchREAD[type(ptr)]( name.encode('utf-8'), byref(ptr), opt.encode('utf-8'))
	app = parse_errh( app1)
	return app

switchPRINT= {	nscf_md_ptr:    _print_nscf_md,
		nscf_data_ptr:  _print_nscf_data,
		opt_data_ptr:   _print_opt_data,
		m_elem_ptr:     _print_m_elem,
		pdos_state_ptr: _print_pdos_state,
		pdos_data_ptr:  _print_pdos_data,
		data_file_ptr:  _print_data_file,
		wfc_ptr:        _print_wfc,
		pseudo_ptr:     _print_pseudo   }
def PRINT( name, ptr):
	file=fopen( name.encode('utf-8'), "w".encode('utf-8'))
	app = switchPRINT[type(ptr)]( ptr, file)
	fclose( file)
	return app

switchFREE= {	nscf_md_ptr:    _free_nscf_md,
		nscf_data_ptr:  _free_nscf_data,
		opt_data_ptr:   _free_opt_data,
		m_elem_ptr:     _free_m_elem,
		pdos_state_ptr: _free_pdos_state,
		pdos_data_ptr:  _free_pdos_data,
		data_file_ptr:  _free_data_file,
		wfc_ptr:        _free_wfc,
		pseudo_ptr:     _free_pseudo   }
def FREE( ptr):
	app = switchFREE[type(ptr)]( ptr)
	#del ptr
	#del ptr.contents
	return app

switchDUPLICATE= {	nscf_md_ptr:    _duplicate_nscf_md,
			nscf_data_ptr:  _duplicate_nscf_data,
			opt_data_ptr:   _duplicate_opt_data,
			m_elem_ptr:     _duplicate_m_elem,
			pdos_state_ptr: _duplicate_pdos_state,
			pdos_data_ptr:  _duplicate_pdos_data,
			data_file_ptr:  _duplicate_data_file,
			wfc_ptr:        _duplicate_wfc,
			pseudo_ptr:     _duplicate_pseudo   }
def DUPLICATE( ptr):
	app = switchDUPLICATE[type(ptr)]( ptr)
	return app

