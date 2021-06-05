

#global testerror;
#global testerror;
#global testerror;
enum A : u8
{
	Red = B.Red;
	Green;
	Blue;
}

//#global testerror;
enum B : u8
{
	Red = C.Red;
	Green;
	Blue;
}

//#global testerror;
enum C : u8
{
	Red = A.Red;
	Green;
	Blue;
}
