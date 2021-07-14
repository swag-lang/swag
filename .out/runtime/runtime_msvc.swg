// Contains ucrt.lib missing functions from libc

#global if #swagos == "windows"
#global if #os == "windows" and #abi == "msvc"
#global foreignlib "ucrt"

private
{
    #[Swag.foreign("kernel32")]
    {
        func RtlCopyMemory(destination: *void, source: const *void, length: uint);
        func RtlMoveMemory(destination: *void, source: const *void, length: uint);
        func RtlCompareMemory(destination, source: const *void, length: uint)->uint;
        func RtlFillMemory(destination: *void, length: uint, fill: s32);
    }
}

////////////////////////////////////////////////////////////
#[Swag.Callback]
{
    func memset(dst: *void, value: s32, size: uint)
    {
        RtlFillMemory(dst, size, value)
    }

    func memcpy(dst: *void, src: const *void, size: uint)
    {
        RtlCopyMemory(dst, src, size)
    }

    func memmove(dst: *void, src: const *void, size: uint)
    {
        RtlMoveMemory(dst, src, size)
    }

    func memcmp(dst, src: const *void, size: uint)->s32
    {
        len := RtlCompareMemory(dst, src, size)
        if len == size return 0
        p1 := cast(const *s8) dst
        p2 := cast(const *s8) src
        return p1[len] - p2[len]
    }
}
