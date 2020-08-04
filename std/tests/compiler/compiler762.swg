struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
#test
{
	cpt := Point
    @assert(cpt == Point)
}
