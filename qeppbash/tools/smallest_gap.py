from ..structures	import *
from ..loader		import *
from ctypes import cast
import numpy as np

def smallest_gap( data=0, radius=0., comp_point=[0.,0.,0.]):
	#data = nscf_data_ptr()
	
	if( radius < 0):
		print( "Invalid negative 'radius'")
		return 1
	if( len(comp_point) != 0):
		if( len(comp_point) != 3):
			print( "'comp_point' should be an [x,y,z] vector")
			return 1
		for x in comp_point:
			try:
				x+=1
			except TypeError:
				print( "Invalid type %s for 'comp_point'" % type(x))
				return 1

	if ( type( data) != nscf_data_ptr()):
		filename = data
		data  = nscf_data_ptr()
		READ( filename, data)

	if( not data):
		print( "NULL pointer")
		return 1

	md       = data[0].md[0]
	energies = data[0].energies
	kpt      = data[0].kpt

	e_fermi = c_double()
	ef = md.e_fermi
	parse_errh( cast(qepp.find_e_fermi( byref(e_fermi), data), errh_ptr))
	print( "E_fermi(from file):\t%f eV" % ef)
	print( "E_fermi(from calc):\t%f eV" % e_fermi.value)

	n_el = md.n_el
	if ( md.spin_orbit):
		vb = (n_el - 1)
		print( "spin-orbit correction detected");
	else:
		vb = (n_el/2 - 1)
		print( "No spin-orbit correction")
	vb = int(vb)
	cb = vb + 1
	print( "vb = %d, cb = %d" % (vb+1, cb+1));

	l   = []
	lcb = []
	lvb = []
	lg1 = []
	lg2 = []
	lg3 = []
	cp = np.array( comp_point)
	if( radius > 0):
		gen = ( i for i in range( md.n_kpt) if (np.linalg.norm(cp-np.array( kpt[i][0:3])) < radius))
	else:
		gen = ( i for i in range( md.n_kpt))
	for i in gen:
		l.append(   i)
		lcb.append( energies[i][cb])
		lvb.append( energies[i][vb])
		lg1.append( energies[i][cb]   - energies[i][vb])
		lg2.append( energies[i][cb+1] - energies[i][vb])
		lg3.append( energies[i][cb+1] - energies[i][cb])

	m1 = max( lvb)
	mg1 = l[ lvb.index( m1)]
	print( "\nMax_vb_energy: vb= %lf eV" % m1);
	print("\tat %f %f %f (2pi/a) (# %li) " % (kpt[mg1][0], kpt[mg1][1], kpt[mg1][2], mg1+1));

	m2 = min( lcb)
	mg2 = l[ lcb.index( m2)]
	print( "\nMin_cb_energy: cb= %lf eV" % m2);
	print("\tat %f %f %f (2pi/a) (# %li) " % (kpt[mg2][0], kpt[mg2][1], kpt[mg2][2], mg2+1));

	if( mg1 == mg2):
		print( "DIRECT GAP %lf eV" % (m2 - m1))
	else:
		if( m2 < m1):
			print( "METALLIC")
		else:
			print( "INDIRECT GAP %lf eV" % (m2 - m1))

	if( ef == ef):
		m = min( i for i in lg1 if lvb[ lg1.index( i)] < ef < lcb[ lg1.index(i )])
		mog = l[ lg1.index( m)]
		print("\nMin_opt_gap: %lf eV\n\tat   %lf %lf %lf (2pi/a) (# %li)" % (m, kpt[mog][0], kpt[mog][1], kpt[mog][2], mog+1))
		print("\t%lf -> %lf   Ef: %lf eV" % (energies[mog][vb], energies[mog][vb+1], ef))		
	else:
		print( "\nCannot calculate min_opt_gap with invalid fermi energy")

	m = min( lg1)
	mg = l[ lg1.index( m)]
	print("\nMin gap energy: %lf eV" % m);
	print("\tat %f %f %f (2pi/a) (# %li) " % (kpt[mg][0], kpt[mg][1], kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (energies[mg][vb], energies[mg][vb+1], ef));

	m = min( lg2)
	mg = l[ lg2.index( m)]
	print("\nMin gap energy(vb->cb+1): %lf eV" % m);
	print("\tat %f %f %f (2pi/a) (# %li)" % (kpt[mg][0], kpt[mg][1], kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (energies[mg][vb], energies[mg][vb+2], ef));

	m = min( lg3)
	mg = l[ lg3.index( m)]
	print("\nMin gap energy(cb->cb+1): %lf eV" % m);
	print("\tat %f %f %f (2pi/a) (# %li)" % (kpt[mg][0], kpt[mg][1], kpt[mg][2], mg+1));
	print("\t%lf -> %lf   Ef: %lf eV" % (energies[mg][cb], energies[mg][vb+2], ef));

	if ( type( data) != nscf_data_ptr()):
		FREE( data)

	return 0
	
'''
	mve = c_long()
	mce = c_long()
	parse_errh( cast(qepp.find_max_v_bnd( byref(mve), data), errh_ptr))
	parse_errh( cast(qepp.find_min_c_bnd( byref(mce), data), errh_ptr))
	mve = mve.value
	mce = mce.value
	#print( mve, mce, vb, type(mve), type(mce), type(vb))
	if( mve >=0):
		print( "\nMax_vb_energy: vb= %lf cb= %lf eV" % (data[0].energies[mve][vb], data[0].energies[mve][vb+1]));
		print( "\tat   %lf %lf %lf (2pi/a) (# %li)" % (data[0].kpt[mve][0],data[0].kpt[mve][1],data[0].kpt[mve][2],mve+1));
	else:
		print( "Max_vb_energy: FAILED\n");

	if( mce >=0):
		print("Min_cb_energy: vb= %lf cb= %lf eV" % (data[0].energies[mce][vb],data[0].energies[mce][vb+1]));
		print("\tat   %lf %lf %lf (2pi/a) (# %li)" % (data[0].kpt[mce][0],data[0].kpt[mce][1],data[0].kpt[mce][2],mce+1));
	else:
		print("Min_cb_energy: FAILED\n");


	mog = c_long()
	parse_errh( cast(qepp.find_min_opt_gap( byref(mog), data), errh_ptr))
	mog = mog.value
	if( mog >= 0):
		min_opt_gap = data[0].energies[mog][vb+1] - data[0].energies[mog][vb];
		print("\nMin_opt_gap: %lf eV\n\tat   %lf %lf %lf (2pi/a) (# %li)" % (min_opt_gap, data[0].kpt[mog][0], data[0].kpt[mog][1],
		data[0].kpt[mog][2], mog+1));
		print("\t%lf -> %lf   Ef: %lf eV" % (data[0].energies[mog][vb], data[0].energies[mog][vb+1], ef));
	else:
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

	if ( type( data) != nscf_data_ptr()):
		FREE( data)
'''


#def find_e_fermi():


#def find_max_vb():


#def find_min_cb():




















