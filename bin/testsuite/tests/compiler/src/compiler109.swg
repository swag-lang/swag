#global testerror "Err0515"
enum A : u8
{
	Red = B.Red;
	Green;
	Blue;
}

enum B : u8
{
	Red = A.Red;
	Green;
	Blue;
}
