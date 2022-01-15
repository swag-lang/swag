using Core, StrConv

struct(T) SoaTest
{
    #ast
    {
        var sb: StringBuilder
        visit p: @typeof(T).fields
            sb.appendFormat("%: Array'%\n", p.name, p.pointedType.flatName)
        return sb.moveToString()
    }
}

impl SoaTest
{
    #run
    {
        itf := @compiler()
        var sb: StringBuilder
        itf.compileString(sb.moveToString())
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