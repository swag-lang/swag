struct Vector2
{
	x, y: f32
}

const One: Vector2{1, 1}

func dot(other: Vector2) {}


//#[swag.printbc]
#test
{
	dot(One)
}