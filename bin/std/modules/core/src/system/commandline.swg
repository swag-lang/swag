#global skip
#global namespace CommandLine

struct(T) IsSet
{
    #ast
    {
        str := StrConv.StringBuilder{}
        typeof := @typeof(T)
        visit f: typeof.fields
            str.appendFormat("%: bool\n", f.name)
        return str.toString()
    }
}

struct ParseOptions
{
    argumentMarker = "-"'u8
}

func(T) parse(st: *T, args: const [..] string, options = ParseOptions{})->IsSet
{
    var x: IsSet

    typeof := @typeof(T)

    for itp := 0; itp < @countof(args); itp += 1
    {
        oneArg := args[itp]

        if !@countof(oneArg) or oneArg[0] != "-"'u8//options.argumentMarker
            continue
        oneArg = Tokenize.eatCount(oneArg, 1)

        typeField := Reflection.getField(typeof, oneArg)
        if !typeField
            continue

        addr := cast(*u8) st
        addr += typeField.offset

        itpField := itp
        switch typeField.pointedType
        {
        case s32, f32:
            discard catch Parse.pokeValue(addr, typeField.pointedType, args[itp + 1])
            itp += 1
        }

        typeFieldIsSet := Reflection.getField(@typeof(x), args[itpField])
        if typeFieldIsSet
        {
            addrIsSet := cast(*bool) &x
            addrIsSet += typeFieldIsSet.offset
            dref addrIsSet = true
        }
    }

    return x
}

#run
{
    struct TT
    {
        _s32: s32
        _f32: f32
    }

    var x: TT
    params := @["-_s32", "10", "-_f32", "3.14"]
    a := parse(&x, params)

    Console.print(x)
    Console.print(a)
}