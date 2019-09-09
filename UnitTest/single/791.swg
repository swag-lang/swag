#module module791

var cpt: u32 = 0

//#[swag.printbc]
func(T) toto()
{
	t := @typeof(T)
	if(t == @typeof(s32))
		cpt |= 1
	if(t == @typeof(f32))
		cpt |= 2
}


//#[swag.printbc]
#test
{
	cpt = 0
	toto!s32()
	toto!f32()
	@assert(cpt == 3)
}