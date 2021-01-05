#global testerror

//#[swag.enumflags]
enum Days
{
	A = 1
}

#test
{
	x := ~Days.A
}