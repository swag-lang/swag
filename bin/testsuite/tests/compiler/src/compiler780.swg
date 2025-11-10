#test
{
    using Swag

    let t = #typeof(u32)
    @assert(t.kind == TypeInfoKind.Native)
    @assert(t.sizeof == 4)

    let ptr = cast(const *TypeInfoNative) t
    @assert(ptr.nativeKind == TypeInfoNativeKind.U32)
}
