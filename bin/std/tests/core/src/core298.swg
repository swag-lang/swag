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

    typeof := @typeof(TT)

    for itp := 0; itp < @countof(args); itp += 2
    {
        typeField := typeof.fields[0]
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
    var x: TT
    params := @["_f32", "3.14"]
    a := parse(&x, params)
}