from ctypes import CDLL, Structure, c_char, c_char_p, c_int, c_uint, c_long, c_ulong, c_double, c_bool, c_void_p, POINTER, byref

qepp = CDLL( 'libqepp.so')


####################################################################################################
#C files
class _FILE(Structure):
	pass
FILE_ptr = POINTER( _FILE)
fopen                = qepp.fopen
qepp.fopen.argtypes  = [ c_char_p, c_char_p]
qepp.fopen.restype   = FILE_ptr
fclose               = qepp.fclose
qepp.fclose.argtypes = [ FILE_ptr]
####################################################################################################

####################################################################################################
# qepp error handler
class _errh(Structure):
	_fields_ = [ ('cname',      c_char * 256),
		     ('ext',        c_bool),
		     ('extra',      c_char * 256),
		     ('ecode',      c_int)]
errh_ptr = POINTER( _errh)
parse_errh          = qepp.parse_errh
parse_errh.argtypes = [ errh_ptr]
parse_errh.restype  = c_int
####################################################################################################

