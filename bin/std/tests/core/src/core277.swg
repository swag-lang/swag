using Core

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
    cur:     typeinfo
    buf:     [maxSize(T)] u8
}

impl AA
{
    #[Swag.ConstExpr]
    func isIn(V: typeinfo)->bool
    {
        var ms: u64 = 0
        foreach p in T:
            if p == V:
                return true
        return false
    }

    mtd(TV) set(v: TV)
    {
        #if !isIn(TV):
            #error("invalid type " ++ #stringof(#typeof(TV)))

        if me.cur and me.cur != TV and me.cur.flags & Swag.TypeInfoFlags.HasDrop
        {
            let typeStruct = cast(const *Swag.TypeInfoStruct) me.cur
            typeStruct.opDrop(me.buf)
            @init(cast(*TV) me.buf, 1)
        }

        me.cur                = TV
        dref cast(*TV) me.buf = v
    }

    mtd(TV) get()->TV
    {
        #if !isIn(TV):
            #error("invalid type " ++ #stringof(#typeof(TV)))
        @assert(me.cur == TV)
        return dref cast(*TV) me.buf
    }
}

#test
{
    var v: AA'([s32, String])

    let str: String = "5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 "
    v.set'String(str)

    v.set's32(666)
    let x = v.get's32()
    @assert(x == 666)
}
