#module module784

struct Point
{
	x: f32
	y: f32
}

#test
{
	using swag
	
	t := @typeof(Point)
	@assert(t.name == "Point")
	
	ptr := cast(const *TypeInfoStruct) t;
	@assert(cast(s32) @countof(ptr.fields) == 2)
}