#global testerror "Err0848"

enum OT
{
	A = 1
	B
}

//#[Swag.PrintBc]
#test
{
	var pt = #type {x: OT}
}
