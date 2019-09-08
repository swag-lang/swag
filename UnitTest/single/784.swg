#module module784

struct Point
{
	x: f32
	y: f32
	z: f32
}

#test
{
	using swag
	
	t := @typeof(Point)
	@assert(t.name == "Point")
	@assert(t.kind == TypeInfoKind.Struct)
	
	ptr := cast(const *TypeInfoStruct) t;
	@assert(@countof(ptr.fields) == 3)
}