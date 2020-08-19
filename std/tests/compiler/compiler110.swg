#module "compiler110"

#unittest error;
#unittest error;
#unittest error;
enum A : u8
{
	Red = B.Red;
	Green;
	Blue;
}

//#unittest error;
enum B : u8
{
	Red = C.Red;
	Green;
	Blue;
}

//#unittest error;
enum C : u8
{
	Red = A.Red;
	Green;
	Blue;
}
