using Core, Swag

struct Defined
{
    xx: s32
}

struct TT
{
    _f32: f32
}

//#[Swag.PrintBc]
func parse(st: *TT, args: const [..] string)->Defined
{
    var x: Defined

    let typeof = @typeof(TT)

    for var itp = 0; itp < @countof(args); itp += 2
    {
        var typeField = typeof.fields[0]
        switch typeField.pointedType
        {
        case s32:
            break
        case f32:
            break
        }
    }

    return x
}

//#[Swag.PrintBc]
#run
{
    var x:     TT
    var params = ["_f32", "3.14"]
    var a      = parse(&x, params)
}
