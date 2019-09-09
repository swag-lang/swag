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
	@assert(t.kind == typeinfo_kind.Struct)
	
	ptr := cast(const *typeinfo_struct) t;
	@assert(@countof(ptr.fields) == 3)
	
	ptr0 := ptr.fields[0]
	@assert(ptr0.base.kind == typeinfo_kind.Param)
	@assert(ptr0.named_param == "x")
	@assert(ptr0.pointed_type.kind == typeinfo_kind.Native)
	@assert(ptr0.offset == 0)
	
	ptr1 := ptr.fields[1]
	@assert(ptr1.named_param == "y")
	@assert(ptr1.offset == 4)
}