


var c: s32 = 0

//#[Swag.printbc]
#test
{
	op := "toto10"
	if(op == "toto")
		c = 1
	else if(op == "toto1")
		c = 2
	@assert(c == 0)
}