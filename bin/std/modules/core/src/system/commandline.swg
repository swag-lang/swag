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

// Returns true if `oneArg` is a valid option (starting with a delimiter)
func isOption(oneArg: string, options: ParseOptions)->bool
{
    if @countof(oneArg) == 0
        return false
    return oneArg[0] == options.optionDelimiter
}

// Clean and split a list of arguments
// -option:value or -option=value => -option value
func splitArguments(args: const [..] string, options: ParseOptions)->Array'string
{
    var allArgs: retval

    // Clean all arguments
    visit oneArg: args
    {
        oneArg = Tokenize.eatSpaces(oneArg)
        if !@countof(oneArg)
            continue

        if !isOption(oneArg, options)
        {
            allArgs.add(oneArg)
            continue
        }

        split := Tokenize.split(oneArg, @[":"'rune, "="'rune], 1)
        if split.count == 2
        {
            allArgs.add(split[0])
            other := Tokenize.eatSpaces(split[1])
            if @countof(other) allArgs.add(other)
        }
        else
        {
            allArgs.add(oneArg)
        }
    }

    return allArgs
}

func(T) parse(st: *T, args: const [..] string, options = CommandLine.ParseOptions{})->Result throw
{
    var result:  retval

    // First pass
    allArgs := splitArguments(args, options)

    // Parse all
    typeof := @typeof(T)
    maxItp := @countof(allArgs)
    for itp := 0; itp < maxItp; itp += 1
    {
        oneArg := allArgs[itp]
        if !isOption(oneArg, options)
            continue
        oneArg = Tokenize.eatCount(oneArg, 1)

        typeField := Reflection.getField(typeof, oneArg)
        if !typeField
            throw Format.toString("unknown option '%'", oneArg)

        addr := cast(*u8) st
        addr += typeField.offset

        // String
        if Reflection.isString(typeField.pointedType)
        {
            if itp + 1 == maxItp or isOption(allArgs[itp + 1], options)
                throw Format.toString("missing value for option '%'", oneArg)

            dref cast(*string) addr = allArgs[itp + 1]
            itp += 1
        }

        // Integer or float
        else if Reflection.isInteger(typeField.pointedType) or Reflection.isFloat(typeField.pointedType)
        {
            if itp + 1 == maxItp or isOption(allArgs[itp + 1], options)
                throw Format.toString("missing value for option '%'", oneArg)

            rest := catch Parse.pokeValue(addr, typeField.pointedType, allArgs[itp + 1])

            if @err
                throw Format.toString("invalid value for option '%' (%)", oneArg, @err)
            if @countof(rest)
                throw Format.toString("invalid value for option '%'", oneArg)
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
