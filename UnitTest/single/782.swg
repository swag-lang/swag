#module module782

#test
{
	using swag
	
	t := @op(*u32, typeof)
	@assert(t.kind == TypeInfoKind.Pointer)
	@assert(t.sizeOf == @op(*void, sizeof))
	
	ptr := cast(const *TypeInfoPointer) t;
	@assert(ptr.ptrCount == 1)
	
	ptr1 := ptr.pointedType;
	@assert(ptr1.kind == TypeInfoKind.Native)
}