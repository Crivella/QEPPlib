#__all__ = [ "main", "structures", "utils" ]

from .structures	import *
from .functions		import *	
from .utils		import using

import qeppbash.tools	

import atexit

def quit_nicely():
	close_io_env()

atexit.register(quit_nicely)

open_io_env(0,0,5)
