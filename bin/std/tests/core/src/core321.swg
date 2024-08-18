using Core, Core.StrConv

struct(T) SoaTest
{
    #ast
    {
        var sb: StringBuilder
        visit p in @typeof(T).fields:
            sb.appendFormat("%: Array'%\n", p.name, p.pointedType.name)
        return sb.toString()
    }
}

impl SoaTest
{
    #run
    {
        let itf = @compiler()
        var sb: StringBuilder
        itf.compileString(sb.toString())
    }
}

#test
{
    struct Test
    {
        x, y, z: f32
    }

    var v: SoaTest'Test
    v.x.reserve(10)
    v.y.reserve(10)
    v.z.reserve(10)
}
