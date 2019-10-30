var g: bool = false

#[swag.inline]
func toto()->s32
{
	if(g)
		return 1
	else
		return 0
}

//#[swag.printbc]
#test
{
	g = true
	@assert(toto() == 1)
	g = false
	@assert(toto() == 0)
}