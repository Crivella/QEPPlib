from qeppbash.structures	import *
from qeppbash.loader		import *
from ctypes import cast

def smallest_gap( data=0, radius=0, comp_point=[]):
	#data = nscf_data_ptr()
	if( radius < 0) :
		print( "Invalid negative 'radius'")
		return 1
	if( len(comp_point) != 0) :
		if( len(comp_point) != 3) :
			print( "'comp_point' should be an [x,y,z] vector")
			return 1
		for x in comp_point:
			try:
				x+=1
			except TypeError:
				print( "Invalid type %s for 'comp_point'" % type(x))
				return 1


	#READ( filename, data)

	e_fermi = c_double()
	ef = data[0].md[0].e_fermi
	parse_errh( cast(qepp.find_e_fermi( byref(e_fermi), data), errh_ptr))
	print( "E_fermi(from file):\t%f eV" % ef)
	print( "E_fermi(from calc):\t%f eV" % e_fermi.value)

	n_el = data.contents.md.contents.n_el
	if ( data[0].md[0].spin_orbit) :
		vb = (n_el - 1)
		print( "spin-orbit correction detected");
	else :
		vb = (n_el/2 - 1)
		print( "No spin-orbit correction")
	vb = int(vb)
	cb = vb + 1
	print( "vb = %d, cb = %d" % (vb+1, cb+1));

	mve = c_long()
	mce = c_long()
	parse_errh( cast(qepp.find_max_v_bnd( byref(mve), data), errh_ptr))
	parse_errh( cast(qepp.find_min_c_bnd( byref(mce), data), errh_ptr))
	mve = mve.value
	mce = mce.value
	#print( mve, mce, vb, type(mve), type(mce), type(vb))
	if( mve >=0) :
		print( "\nMax_vb_energy: vb= %lf cb= %lf eV" % (data[0].energies[mve][vb], data[0].energies[mve][vb+1]));
		print( "\tat   %lf %lf %lf (2pi/a) (# %li)" % (data[0].kpt[mve][0],data[0].kpt[mve][1],data[0].kpt[mve][2],mve+1));
	else :
		print( "Max_vb_energy: FAILED\n");

	if( mce >=0) :
		print("Min_cb_energy: vb= %lf cb= %lf eV" % (data[0].energies[mce][vb],data[0].energies[mce][vb+1]));
		print("\tat   %lf %lf %lf (2pi/a) (# %li)" % (data[0].kpt[mce][0],data[0].kpt[mce][1],data[0].kpt[mce][2],mce+1));
	else :
		print("Min_cb_energy: FAILED\n");


	mog = c_long()
	parse_errh( cast(qepp.find_min_opt_gap( byref(mog), data), errh_ptr))
	mog = mog.value
	if( mog >= 0) :
		min_opt_gap = data[0].energies[mog][vb+1] - data[0].energies[mog][vb];
		print("\nMin_opt_gap: %lf eV\n\tat   %lf %lf %lf (2pi/a) (# %li)" % (min_opt_gap, data[0].kpt[mog][0], data[0].kpt[mog][1],
		data[0].kpt[mog][2], mog+1));
		print("\t%lf -> %lf   Ef: %lf eV" % (data[0].energies[mog][vb], data[0].energies[mog][vb+1], ef));
	else :
		print("\nMin_opt_gap: FAILED\n");

	mg = c_int()
	parse_errh( cast(qepp.find_smallest_gap( byref(mg), data), errh_ptr))
	mg = mg.value
	mge = data[0].energies[mg][cb] - data[0].energies[mg][vb]
	print("\nMin gap energy: %lf eV" % mge);
	print("\tat %f %f %f (2pi/a) (# %li) " % (data[0].kpt[mg][0], data[0].kpt[mg][1], data[0].kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (data[0].energies[mg][vb], data[0].energies[mg][vb+1], ef));

	mg = c_int()
	parse_errh( cast(qepp.find_smallest_gap2( byref(mg), data), errh_ptr))
	mg = mg.value
	mge = data[0].energies[mg][cb+1] - data[0].energies[mg][vb];
	print("\nMin gap energy(vb->cb+1): %lf eV" % mge);
	print("\tat %f %f %f (2pi/a) (# %li)" % (data[0].kpt[mg][0], data[0].kpt[mg][1], data[0].kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (data[0].energies[mg][vb], data[0].energies[mg][vb+2], ef));
	
	mg = c_int()
	parse_errh( cast(qepp.find_smallest_gap3( byref(mg), data), errh_ptr))
	mg = mg.value
	mge = data[0].energies[mg][cb+1] - data[0].energies[mg][cb];
	print("\nMin gap energy(cb->cb+1): %lf eV" % mge);
	print("\tat %f %f %f (2pi/a) (# %li)" % (data[0].kpt[mg][0], data[0].kpt[mg][1], data[0].kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (data[0].energies[mg][cb], data[0].energies[mg][vb+2], ef));

	
	return 0
