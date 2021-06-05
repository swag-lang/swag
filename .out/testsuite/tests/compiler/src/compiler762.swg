struct Point
{
	x: s32
	y: s32
}

//#[Swag.printbc]
#test
{
	cpt := Point
    @assert(cpt == Point)
}
