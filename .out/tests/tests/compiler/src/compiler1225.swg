struct Vector2
{
	x: s32
	y: s32
}

impl Vector2
{
    //#[Swag.printbc]
    func normalize(using self)->Vector2
    {
        return Vector2{x + 1, y + 1}
    }
}

//#[Swag.printbc]
#test
{
	var pt: Vector2{5, 100}
	var pt1: Vector2 = pt.normalize()
    @assert(pt1.x == 6)
    @assert(pt1.y == 101)
}
