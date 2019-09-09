#module module780

#test
{
	using swag
	
	t := @typeof(u32)
	@assert(t.kind == typeinfo_kind.Native)
	@assert(t.sizeof == 4)
	
	ptr := cast(const *typeinfo_native) t;
	@assert(ptr.native_kind == typeinfo_native_kind.U32)
}