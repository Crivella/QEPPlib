import resource
def using(point=""):
	usage=resource.getrusage(resource.RUSAGE_SELF)
	return '''%s: usertime=%s systime=%s mem=%s b
	       '''%(point,usage[0],usage[1],
	            (usage[2]*resource.getpagesize())/1.0 )
