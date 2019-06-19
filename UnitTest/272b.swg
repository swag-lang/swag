#unittest module module272b;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#unittest backend dll;

	#[swag.public]
	func other: ()
	{
		//@print("::::::::::::::::::::::::::::::::::");
	}
}