#global testerror "Err0640"

//#[Swag.EnumFlags]
enum Days
{
	A = 1
}

#test
{
	var x = ~Days.A
}