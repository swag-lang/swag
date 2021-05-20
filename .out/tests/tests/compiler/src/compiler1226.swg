struct Vector2
{
	x: s32
	y: s32
}

func normalize()->Vector2
{
	return Vector2{1, 1}
}

impl Vector2
{
	func toto(self)
	{
	}
}

//#[Swag.printbc]
#test
{
	pt1 := normalize()
	//Vector2.toto(pt1)
}
