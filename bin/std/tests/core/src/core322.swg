//#global skip
using Core, Core.StrConv

struct(T) SoaTest
{
    #ast
    {
        var sb: StringBuilder
        foreach p in #typeof(T).fields:
            sb.appendFormat("%: Array'%\n", p.name, p.pointedType.name)
        return sb.toString()
    }
}

impl SoaTest
{
    mtd reserve(size: u64)
    {
        #ast
        {
            var sb: StringBuilder
            foreach p in #typeof(T).fields:
                sb.appendFormat("me.%.reserve(size)\n", p.name)
            return sb.toString()
        }
    }
}

#test
{
    struct Test { x, y, z: f32 }

    var v: SoaTest'Test
    v.reserve(10)
    @assert(v.x.capacity == 10)
    @assert(v.y.capacity == 10)
    @assert(v.z.capacity == 10)
}
