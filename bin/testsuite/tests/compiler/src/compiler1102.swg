#global testerror "Err0650"

//#[Swag.EnumFlags]
enum Days
{
	A = 1
}

#test
{
	var x = ~Days.A
}