from os      import listdir
from os.path import isfile, join
from re      import split
from numpy   import array, zeros, loadtxt



orb_types = ( 's', 'p', 'd', 'f') 
max_orb = len( orb_types)

def sum_pdos( atoms="", mypath=".", plot=False):
	if( not atoms):
		print( "Passing empty name for 'atoms'")
		return 1

	check = False
	n_pt = 0
	atoms_l = atoms.split(",")
	max_atm = len( atoms_l)

	for atom, j in zip( atoms_l, range( len( atoms_l))):
		for f in listdir( mypath):
			if( "pdos_atm#" in f and "({})".format( atom) in f):
				name = join( mypath, f)
				print( "READING: {}".format( name))
				data = loadtxt( fname=name, comments="#")

				n_pt = len( data)
				if( not check):
					check = True
					res_x   = data[:,0]
					res_orb = zeros(( max_atm, n_pt, max_orb))
				l = list(filter( None, split( ".+atm#|\(.+#|\(|\)|\_j", f)))
				atm_n = j
				wfc_n = int( l[1])-1
				try:
					orb_n = int( orb_types.index( l[2]))
				except ValueError:
					print( "Non recognize orbital type {}".format( l[2]))
					return 1

				res_orb[atm_n,:,orb_n] += data[:,1]

	if( check):
		for atom, j in zip( atoms_l, range( len( atoms_l))):
			fout2 = open( "{}_total_pdos_orb.dat".format( atom), "w")
			fout2.write( "#{:>12}".format( "Energy(eV)"))
			for i in range( max_orb):
				fout2.write( "{:>12}_{}".format( atom, orb_types[i]))
			fout2.write( "\n")
			for i in range( n_pt):
				fout2.write( "{:13.8f}".format( res_x[i]))
				for item in res_orb[j,i,:]:
					fout2.write( "{:14g}".format( item))
				fout2.write( "\n")
			fout2.close()

		if plot:
			import matplotlib.pyplot as plt
			from matplotlib.ticker import AutoMinorLocator as AML
			fig, ax = plt.subplots()
			for atom, j in zip( atoms_l, range( len( atoms_l))):
				for y, label in zip( res_orb[j].transpose(), orb_types):
					plt.plot( res_x, y, label="{}_{}".format( atom, label))
			plt.ylabel( "PDOS arb. units")
			plt.xlabel( "Energy( eV)")
			ml1 = AML(5)
			ml2 = AML(5)
			ax.yaxis.set_minor_locator(ml1)
			ax.yaxis.set_tick_params(which='both', right = True)
			ax.xaxis.set_minor_locator(ml2)
			ax.xaxis.set_tick_params(which='both', top = True)
			plt.legend()
			plt.show()
		
	return 0


if __name__ == "__main__":
	import sys
	argc = len( sys.argv)
	if( not 2<=argc<=4):
		print("Incorrect use. Pleas pass arguments:"
			"\n\t'atom_list\t(comma separated)',"
			"\n\t'path\t\t(default = \".\")',"
			"\n\t'plot\t\t(default = False)'")
		exit()
	if( argc==2):
		sum_pdos( sys.argv[1])
	if( argc==3):
		sum_pdos( sys.argv[1], sys.argv[2])
	if( argc==4):
		sum_pdos( sys.argv[1], sys.argv[2], sys.argv[3])










