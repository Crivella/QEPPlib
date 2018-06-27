import sys
import os.path
import re


if "qeppbash" in sys.modules:
	from ..structures	import *
	from ..loader		import *
else:
	from qeppbash.structures import *
	from qeppbash.loader	 import *

def pdos_char( fname="", kpt_list=[], bnd_list=""):
	if( not os.path.isfile(fname)):
		print( "ERROR: File '{}' does not exist.".format( fname))
		return 1
	if( not kpt_list):
		print( "ERROR: Passing empty kpt_list.")
		return 1
	if( isinstance( kpt_list, str)):
		kl = list( map( int, kpt_list.split(",")))
	if( isinstance( kpt_list, list)):
		kl = kpt_list
	
	fkl     = []
	state_l = []

	c = -1
	check = False
	for line in open( fname):
		line = line.rstrip()
		if( " state #" in line):
			state_l.append( line)
		if( " k = " in line or check):
			#print( line)
			if( " k = " in line):
				fkl.append( list( map( float, list( filter( None, line.split( " ")))[2:5])))
				print( fkl)
				return 0
				if( c>0):
					print( fkl[c])
					return 0
				fkl.append( line)
				c+=1
			#if( not check):
			#	fkl.append( line)
			else:
				fkl[c] += line
			check = True

	return 0



if __name__ == "__main__":
	import sys
	argc = len( sys.argv)
	if( not 2<=argc<=4):
		print("Incorrect use. Pleas pass arguments:"
			"\n\t'fname',"
			"\n\t'kpt_list\t(comma separated)',"
			"\n\t'bnd_list\t(optional) (comma separated)'")
		exit()
	if( argc==2):
		pdos_char( sys.argv[1])
	if( argc==3):
		pdos_char( sys.argv[1], sys.argv[2])
	if( argc==4):
		pdos_char( sys.argv[1], sys.argv[2], sys.argv[3])






