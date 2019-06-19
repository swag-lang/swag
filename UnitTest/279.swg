#unittest module module279;
#unittest pass semantic;

#version(toto)
{
	func a: ()
	{
		@assert(false);
	}
}
#else
{
	func a: ()
	{
	}
}

#run a;