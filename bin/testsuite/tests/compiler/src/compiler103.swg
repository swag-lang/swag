#global testerror "Err0453";

enum A: bool
{
	Red = true;
	Green = false;
	Blue = true;
}

enum B: bool
{
	Red = true;
	Green;
	Blue = true;
}
