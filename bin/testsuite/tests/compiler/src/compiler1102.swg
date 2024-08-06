#global testerror "Err0647"

//#[Swag.EnumFlags]
enum Days
{
	A = 1
}

#test
{
	var x = ~Days.A
}