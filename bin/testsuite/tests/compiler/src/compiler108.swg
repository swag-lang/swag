#global testerror "Saf0025"

enum A : u8
{
	Red = B.Red;
	Green;
	Blue;
}

enum B : s32
{
	Red = 256;
	Green;
	Blue;
}
