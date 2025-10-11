#[Swag.CanOverflow(true)]
func mulU64(x, y: u64)->{ hi: u64, lo: u64 }
{
    const mask32 = (1'u64 << 32) - 1
    let x0       = x & mask32
    let x1       = x >> 32
    let y0       = y & mask32
    let y1       = y >> 32
    let w0       = x0 * y0
    let t        = (x1 * y0) + (w0 >> 32)
    var w1       = t & mask32
    let w2       = t >> 32
    w1 += x0 * y1

    var result: retval
    result.hi = (x1 * y1) + w2 + (w1 >> 32)
    result.lo = x * y
    return result
}

#test
{
    let a = mulU64(14381545078898526464, 11830521861667747109)
    @assert(a.hi == 9223372036854775295)
    @assert(a.lo == 13919103551811865856)
}
