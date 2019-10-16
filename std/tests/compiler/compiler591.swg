


var c: s32 = 0

//#[swag.printbc]
#test
{
	op := "toto1"
	if(op == "toto")
		c = 1
	else if(op == "toto1")
		c = 2	
	@assert(c == 2)
}