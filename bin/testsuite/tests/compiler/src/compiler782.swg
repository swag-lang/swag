#test
{
    using Swag

    let t = #typeof(*u32)
    @assert(t.kind == TypeInfoKind.Pointer)
    @assert(t.sizeof == #sizeof(*void))

    let ptr = cast(const *TypeInfoPointer) t

    let ptr1 = ptr.pointedType
    @assert(ptr1.kind == TypeInfoKind.Native)

    let ptr2 = cast(const *TypeInfoNative) ptr1
    @assert(ptr2.nativeKind == TypeInfoNativeKind.U32)
}
