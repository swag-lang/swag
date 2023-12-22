using Core

#[Swag.ConstExpr]
func maxSize(T: const [..] typeinfo)->u64
{
    var ms: u32 = 0
    visit p: T
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
        visit p: T
            if p == V return true
        return false
    }

    mtd(TV) set(v: TV)
    {
        #if !isIn(TV) #do #error "invalid type " ++ @stringof(@typeof(TV))

        if cur and cur != TV and cur.flags & Swag.TypeInfoFlags.HasDrop
        {
            let typeStruct = cast(const *Swag.TypeInfoStruct) cur
            typeStruct.opDrop(buf)
            @init(cast(*TV) buf)
        }

        cur = TV
        dref cast(*TV) buf = v
    }

    mtd(TV) get()->TV
    {
        #if !isIn(TV) #do #error "invalid type " ++ @stringof(@typeof(TV))
        @assert(cur == TV)
        return dref cast(*TV) buf
    }
}

#test
{
    var v: AA'([s32, String])

    var str: String = "5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 5555 "
    v.set'String(str)

    v.set's32(666)
    let x = v.get's32()
    @assert(x == 666)
}