struct Vector2
{
	x, y: f32
}

const One: Vector2{1, 1}

func dot(other: Vector2) {}


//#[Swag.printbc]
#test
{
	dot(One)
}