

struct Point
{
	x: f32
	y: f32
	z: f32
}

#test
{
	using Swag

	t := @typeof(Point)
	@assert(t.name == "__compiler784.Point")
	@assert(t.kind == TypeInfoKind.Struct)

	ptr := cast(const *TypeInfoStruct) t;
	@assert(@countof(ptr.fields) == 3)

	ptr0 := ptr.fields[0]
	//@assert(ptr0.base.kind == TypeInfoKind.Param)
	@assert(ptr0.name == "x")
	@assert(ptr0.pointedType.kind == TypeInfoKind.Native)
	@assert(ptr0.offset == 0)

	ptr1 := ptr.fields[1]
	@assert(ptr1.name == "y")
	@assert(ptr1.offset == 4)
}