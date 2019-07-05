#module module266b;
#version(FastTest)
{
	#unittest pass semantic;
}
#else
{
	#unittest backend lib;

	#[swag.public]
	func other: ()
	{
	}
}