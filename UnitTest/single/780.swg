#module module780

#test
{
	using swag
	
	t := @op(u32, typeof)
	@assert(t.kind == TypeInfoKind.Native)
	@assert(t.sizeOf == 4)
	
	ptr := cast(const *TypeInfoNative) t;
	@assert(ptr.nativeKind == TypeInfoNativeKind.U32)
}