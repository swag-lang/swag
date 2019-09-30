#module module990
//#unittest pass semantic
using swag

attr NC1() -> var

struct Point
{
	#[NC1]
	x: s32
	
	#[NC1]
	y: s32
}

//#[swag.printbc]
#test
{
	t := cast(const *typeinfo_struct) @typeof(Point)
	@assert(@countof(t.fields) == 2)
	
	/*loop t.fields
	{
	}*/
}
