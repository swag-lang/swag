#[Swag.ConstExpr]
func maxSize(T: const [..] typeinfo)->u64
{
    var ms: u32 = 0
    foreach p in T do
        ms = @max(p.sizeof, ms)
    return ms
}

struct(T: const [..] typeinfo) AA
{
    buffer: [maxSize(T)] u8
}

#test
{
    var v: AA'([s32, bool])
    @assert(#sizeof(v.buffer) == 4)
    var v1: AA'([s32, bool, f64])
    @assert(#sizeof(v1.buffer) == 8)
}
