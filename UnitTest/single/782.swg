#module module782

#test
{
	using swag
	
	t := @typeof(*u32)
	@assert(t.kind == TypeinfoKind.Pointer)
	@assert(t.sizeof == @sizeof(*void))
	
	ptr := cast(const *TypeinfoPointer) t;
	@assert(ptr.ptr_count == 1)
	
	ptr1 := ptr.pointed_type;
	@assert(ptr1.kind == TypeinfoKind.Native)
	
	ptr2 := cast(const *TypeinfoNative) ptr1;
	@assert(ptr2.native_kind == TypeinfoNativeKind.U32)
}