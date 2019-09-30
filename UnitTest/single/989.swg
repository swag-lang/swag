#module module989
//#unittest pass semantic

attr NC() -> var
attr NC1() -> var

struct Point
{
	#[NC]
	x: s32
	
	#[NC1]
	y: s32
}

//#[swag.printbc]
#test
{
	t := @typeof(Point)
}
