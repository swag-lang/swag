;
#version(FastTest)
{
	#unittest pass backend;
}
#else
{
	attr toto(v: string) -> func;

	#[toto("6")]
	func main ()
	{
	}
}