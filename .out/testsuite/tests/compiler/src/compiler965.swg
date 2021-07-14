struct Point
{
	x: f32
	y: f32
}

func toto()->Point
{
	return @{1, 2}
}

//#[Swag.PrintBc]
#test
{
	pt := toto()
}