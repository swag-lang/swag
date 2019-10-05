#module module780

#test
{
	using swag
	
	t := @typeof(u32)
	@assert(t.kind == TypeinfoKind.Native)
	@assert(t.sizeof == 4)
	
	ptr := cast(const *TypeinfoNative) t;
	@assert(ptr.native_kind == TypeinfoNativeKind.U32)
}