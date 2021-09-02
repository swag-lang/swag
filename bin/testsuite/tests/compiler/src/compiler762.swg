struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
#test
{
	cpt := Point
    @assert(cpt == Point)
}
