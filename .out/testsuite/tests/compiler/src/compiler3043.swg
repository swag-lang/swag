#[Swag.ConstExpr]
func maxSize(T: const [..] typeinfo)->uint
{
    var ms: uint = 0
    visit p: T
        ms = @max(p.sizeof, ms)
    return ms
}

struct(T: const [..] typeinfo) AA
{
    buffer: [maxSize(T)] u8
}

#test
{
    var v: AA'(@[s32, bool])
    @assert(@sizeof(v.buffer) == 4)
    var v1: AA'(@[s32, bool, f64])
    @assert(@sizeof(v1.buffer) == 8)
}