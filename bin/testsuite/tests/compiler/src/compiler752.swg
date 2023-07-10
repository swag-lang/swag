

struct Point
{
	x: s32
	y: s32
}

//#[Swag.PrintBc]
func toto()->Point
{
	var pt: Point{10, 20}
	return pt
}

//#[Swag.PrintBc]
#test
{
    {
        let x = &toto
        var c = x()
        @assert(c.x == 10)
        @assert(c.y == 20)
    }

    {
        var c = toto()
        @assert(c.x == 10)
        @assert(c.y == 20)
    }
}
