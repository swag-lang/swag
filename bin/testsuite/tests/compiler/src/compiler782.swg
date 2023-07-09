

#test
{
	using Swag

	var t = @typeof(*u32)
	@assert(t.kind == TypeInfoKind.Pointer)
	@assert(t.sizeof == @sizeof(*void))

	var ptr = cast(const *TypeInfoPointer) t;

	var ptr1 = ptr.pointedType;
	@assert(ptr1.kind == TypeInfoKind.Native)

	var ptr2 = cast(const *TypeInfoNative) ptr1;
	@assert(ptr2.nativeKind == TypeInfoNativeKind.U32)
}