

#test
{
	using swag

	t := @typeof(*u32)
	@assert(t.kind == TypeInfoKind.Pointer)
	@assert(t.sizeof == @sizeof(*void))

	ptr := cast(const *TypeInfoPointer) t;

	ptr1 := ptr.pointedType;
	@assert(ptr1.kind == TypeInfoKind.Native)

	ptr2 := cast(const *TypeInfoNative) ptr1;
	@assert(ptr2.nativeKind == TypeInfoNativeKind.U32)
}