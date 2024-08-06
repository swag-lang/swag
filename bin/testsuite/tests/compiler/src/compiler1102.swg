#global testerror "Err0339"

//#[Swag.EnumFlags]
enum Days
{
	A = 1
}

#test
{
	var x = ~Days.A
}