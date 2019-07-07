#module module273b;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#unittest backend dll;
	using swag;

	#[public]
	func other()
	{
		//@print("toto");
	}
}