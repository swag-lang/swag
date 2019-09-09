#module module782

#test
{
	using swag
	
	t := @typeof(*u32)
	@assert(t.kind == typeinfo_kind.Pointer)
	@assert(t.sizeof == @sizeof(*void))
	
	ptr := cast(const *typeinfo_pointer) t;
	@assert(ptr.ptr_count == 1)
	
	ptr1 := ptr.pointed_type;
	@assert(ptr1.kind == typeinfo_kind.Native)
	
	ptr2 := cast(const *typeinfo_native) ptr1;
	@assert(ptr2.native_kind == typeinfo_native_kind.U32)
}