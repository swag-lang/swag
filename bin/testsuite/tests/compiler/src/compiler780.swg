

#test
{
	using Swag

	var t = @typeof(u32)
	@assert(t.kind == TypeInfoKind.Native)
	@assert(t.sizeof == 4)

	var ptr = cast(const *TypeInfoNative) t;
	@assert(ptr.nativeKind == TypeInfoNativeKind.U32)
}