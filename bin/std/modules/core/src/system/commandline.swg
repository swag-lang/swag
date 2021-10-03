#global export
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
    optionDelimiter = "-"'u8
}

func(T) parse(st: *T, args: const [..] string, options = ParseOptions{})->Result
{
    var result:  retval
    var allArgs: Array'string

    // Clean all arguments
    visit oneArg: args
    {
        oneArg = Tokenize.eatSpaces(oneArg)
        if !@countof(oneArg)
            continue
        allArgs.add(oneArg)
    }

    // Parse all
    typeof := @typeof(T)
    for itp := 0; itp < @countof(allArgs); itp += 1
    {
        oneArg := allArgs[itp]
        if oneArg[0] != options.optionDelimiter
            continue
        oneArg = Tokenize.eatCount(oneArg, 1)

        typeField := Reflection.getField(typeof, oneArg)
        if !typeField
            continue

        addr := cast(*u8) st
        addr += typeField.offset

        if Reflection.isString(typeField.pointedType)
        {
            dref cast(*string) addr = allArgs[itp + 1]
            itp += 1
        }
        else if Reflection.isInteger(typeField.pointedType) or Reflection.isFloat(typeField.pointedType)
        {
            rest := catch Parse.pokeValue(addr, typeField.pointedType, allArgs[itp + 1])
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
