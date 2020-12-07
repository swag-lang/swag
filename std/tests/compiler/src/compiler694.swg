#module "compiler694"


struct(T) Point
{
	x: T = 5
	y: T = 10
}

#unittest error
type Point2f: Point!bool
