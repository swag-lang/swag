;
#unittest pass semantic;

#version(oula)
	#assert(false);
#else
	#assert(true);
