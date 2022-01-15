using Core, StrConv

struct(T) Soa
{
    #ast
    {
        var sb: StringBuilder
        visit p: @typeof(T).fields
            sb.appendFormat("%: *%\n", p.name, p.pointedType.flatName)
        return sb.moveToString()
    }
}

impl Soa
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

    var v: Soa'Test
}