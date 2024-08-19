#global testerror "Err0631"

//#[Swag.EnumFlags]
enum Days
{
	A = 1
}

#test
{
	var x = ~Days.A
}