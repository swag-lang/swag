#[Swag.ConstExpr]
func maxSize(T: const [..] typeinfo)->u64
{
    var ms: u32 = 0
    foreach p in T:
        ms = @max(p.sizeof, ms)
    return ms
}

struct(T: const [..] typeinfo) AA
{
    cur: typeinfo
    buf: [maxSize(T)] u8
}

impl AA
{
    #[Swag.ConstExpr]
    func isIn(V: typeinfo)->bool
    {
        var ms: u64 = 0
        foreach p in T:
            if p == V: return true
        return false
    }

    mtd(TV) set(v: TV)
    {
        #if !isIn(TV): #error "invalid type " ++ @stringof(@typeof(TV))
        cur = TV
        dref cast(*TV) buf = v
    }

    mtd(TV) get()->TV
    {
        #if !isIn(TV): #error "invalid type " ++ @stringof(@typeof(TV))
        @assert(cur == TV)
        return dref cast(*TV) buf
    }
}

#test
{
    var v: AA'([s32, string])

    v.set's32(666)
    let x = v.get's32()
    @assert(x == 666)

    v.set'string("titi")
    let x1 = v.get'string()
    @assert(x1 == "titi")
}