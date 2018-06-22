#__all__ = [ "main", "structures", "utils" ]

from qeppbash.structures	import *
from qeppbash.functions		import *
from qeppbash.tools		import *
from qeppbash.utils		import using

import atexit

def quit_nicely():
	close_io_env()

atexit.register(quit_nicely)

open_io_env(0,0,5)
