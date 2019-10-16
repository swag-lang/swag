#module compiler103

#unittest pass semantic;

enum A: bool
{
	Red = true;
	Green = false;
	Blue = true;
}

#unittest error;
enum B: bool
{
	Red = true;
	Green;
	Blue = true;
}
