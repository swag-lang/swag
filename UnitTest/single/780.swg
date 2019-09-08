#module module780

#test
{
	using swag
	
	t := @typeof(u32)
	@assert(t.kind == TypeInfoKind.Native)
	@assert(t.sizeOf == 4)
	
	ptr := cast(const *TypeInfoNative) t;
	@assert(ptr.nativeKind == TypeInfoNativeKind.U32)
}