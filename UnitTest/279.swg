#unittest module module279;
#unittest pass semantic;
#version(toto)
{
	func a: ()
	{
	}
}
#else
{
	func a: ()
	{
		@print(2);
	}
}

#run a;