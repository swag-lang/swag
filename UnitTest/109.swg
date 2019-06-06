#unittest module module109;
#unittest pass semantic;

#unittest error;
enum A : u8
{
	Red = B.Red;
	Green;
	Blue;
}

#unittest error;
enum B : u8
{
	Red = A.Red;
	Green;
	Blue;
}
