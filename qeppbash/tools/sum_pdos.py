from ..structures	import *
from ..loader		import *

from os import listdir
from os.path import isfile, join
import re
import numpy as np

orb_types = ( 's', 'p', 'd', 'f') 
max_orb = len( orb_types)

def sum_pdos( atom="", mypath="."):
	if( not atom):
		print( "Passing empty name for 'atom'")
		return 1

	atom = "(%s)" % atom
	#filelist = [f for f in listdir(mypath) if isfile(join(mypath, f))]
	app     = opt_data_ptr()
	res_x   = []
	res_wfc = []
	res_orb = []

	m = []
	for f in listdir( mypath):
		if( "pdos_atm#" in f and atom in f):
			l = list(filter( None, re.split( ".+atm#|\(.+#|\(|\)|\_j", f)))
			m.append( int( l[1]))
	max_wfc = max( m)
	print( m, max_wfc)

	check = False
	n_pt = 0
	for f in listdir( mypath):
		if( "pdos_atm#" in f and atom in f):
			READ( join( mypath, f), app, "#")
			data = app.contents
			n_pt = data.n_pt
			if( not check):
				check = True
				res_x   = np.array( data.x[0:n_pt])
				res_wfc = np.zeros(( n_pt, max_wfc))
				res_orb = np.zeros(( n_pt, max_orb))
			l = list(filter( None, re.split( ".+atm#|\(.+#|\(|\)|\_j", f)))
			atm_n = int( l[0])
			wfc_n = int( l[1])-1
			try:
				orb_n = int( orb_types.index( l[2]))
			except ValueError:
				print( "Non recognize orbital type {}".format( l[2]))
				return 1

			for i in range( n_pt):
				res_wfc[i][wfc_n] += data.values[i][0]
				res_orb[i][orb_n] += data.values[i][0]

			FREE( app)

	atom = atom.strip("()")
	fout1 = open( "{}_total_pdos_wfc.dat".format( atom), "w")
	fout2 = open( "{}_total_pdos_orb.dat".format( atom), "w")
	fout1.write( "#%12s" % "Energy(eV)")
	fout2.write( "#%12s" % "Energy(eV)")
	for i in range( max_wfc):
		fout1.write( "%11s"  % ("wfc_%d" % i))
	for i in range( max_orb):
		fout2.write( "%11s"  % orb_types[i])
	fout1.write( "\n")
	fout2.write( "\n")
	for i in range( n_pt):
		fout1.write( "%13.8f" % res_x[i])
		fout2.write( "%13.8f" % res_x[i])
		for item in res_wfc[i][0:max_wfc]:
			fout1.write( "%11g"  % item)
		for item in res_orb[i][0:max_orb]:
			fout2.write( "%11g" % item)
		fout1.write( "\n")
		fout2.write( "\n")
	fout1.close()
	fout2.close()
		
	return 0











