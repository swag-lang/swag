

struct Point
{
	x: s32
	y: s32
}

//#[swag.printbc]
func toto()->Point
{
	var pt: Point{10, 20}
	return pt
}

//#[swag.printbc]
#test
{
    {
        x := &toto
        c := x()
        @assert(c.x == 10)
        @assert(c.y == 20)
    }

    {
        c := toto()
        @assert(c.x == 10)
        @assert(c.y == 20)
    }
}
