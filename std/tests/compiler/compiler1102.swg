#module compiler1102

//#[swag.flags]
enum Days
{
	A = 1
}

#unittest error
#test
{
	x := ~Days.A
}