from .loader import *

__all__ = [ "open_io_env", "close_io_env"]

'''
qepp_is_file          = qepp.qepp_is_file
qepp_is_file.argtypes = [ c_char_p]
qepp_is_file.restype  = c_bool

qepp_is_dir          = qepp.qepp_is_dir
qepp_is_dir.argtypes = [ c_char_p]
qepp_is_dir.restype  = c_bool

AllocateLinearMem1          = qepp.AllocateLinearMem1
AllocateLinearMem1.argtypes = [ c_ulong, c_long]
AllocateLinearMem1.restype  = c_void_p

FreeLinearMem1          = qepp.FreeLinearMem1
FreeLinearMem1.argtypes = [ c_void_p]
'''

open_io_env           = qepp.open_io_env
open_io_env.argtypes  = [ c_int, c_int, c_int]
close_io_env          = qepp.close_io_env
close_io_env.argtypes = [ ]
