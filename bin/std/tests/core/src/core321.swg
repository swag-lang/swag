using Core, StrConv

struct(T) Soa
{
    #ast
    {
        var sb: StringBuilder
        type := @typeof(T)
        visit p: type.fields
            sb.appendFormat("%: *%\n", p.name, p.pointedType.flatName)
        return sb.moveToString()
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