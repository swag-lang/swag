//#global skip
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

struct(T) Result
{
    isSet: IsSet
}

struct ParseOptions
{
    argumentMarker = "-"'u8
}

func(T) parse(st: *T, args: const [..] string, options = ParseOptions{})->Result
{
    var result: retval

    typeof := @typeof(T)

    for itp := 0; itp < @countof(args); itp += 1
    {
        oneArg := Tokenize.eatSpaces(args[itp])
        if !@countof(oneArg)
            continue

        if oneArg[0] != options.argumentMarker
            continue
        oneArg = Tokenize.eatCount(oneArg, 1)

        typeField := Reflection.getField(typeof, oneArg)
        if !typeField
            continue

        addr := cast(*u8) st
        addr += typeField.offset

        switch typeField.pointedType
        {
        case s8, s32, f32:
            rest := catch Parse.pokeValue(addr, typeField.pointedType, args[itp + 1])
            if @err @print(@err)
            if @countof(rest) @print("rest: ", rest, "\n")
            itp += 1
        }

        typeFieldIsSet := Reflection.getField(@typeof(result.isSet), oneArg)
        if typeFieldIsSet
        {
            addrIsSet := cast(*bool) &result.isSet
            addrIsSet += typeFieldIsSet.offset
            dref addrIsSet = true
        }
    }

    return result
}

#run
{
    struct TT
    {
        value: s8
        diff:  f32
    }

    var x: TT
    params := @["-value", "1.1", "-diff", "3.14"]
    a := parse(&x, params)

    Console.print(x, "\n", a, "\n")
}