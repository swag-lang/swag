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

// Parse all the arguments and fill the result
func(T) parse(st: *T, args: const [..] string, options = CommandLine.ParseOptions{})->Result throw
{
    #[Swag.Mixin]
    func checkNext() throw
    {
        if itp + 1 == maxItp or isOption(allArgs[itp + 1], options)
            throw Format.toString("missing value for option '%'", oneArg)
    }

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

        typeParam := Reflection.getField(typeof, oneArg)
        if !typeParam
            throw Format.toString("unknown option '%'", oneArg)
        typeField := typeParam.pointedType

        addr := cast(*u8) st
        addr += typeParam.offset

        // Bool: Result is true if the option does not have an associated value
        if Reflection.isBool(typeField) and
           (itp + 1 == maxItp or isOption(allArgs[itp + 1], options))
        {
            dref cast(*bool) addr = true
        }

        // String
        else if typeField == @typeof(Core.String)
        {
            try checkNext()
            dref cast(*String) addr = allArgs[itp + 1]
            itp += 1
        }
        else if Reflection.isString(typeField)
        {
            try checkNext()
            dref cast(*string) addr = allArgs[itp + 1]
            itp += 1
        }

        // All types supported by `Parse.pokeValue`
        else if Reflection.isInteger(typeField) or
                Reflection.isFloat(typeField) or
                Reflection.isBool(typeField) or
                Reflection.isEnum(typeField)
        {
            try checkNext()
            rest := catch Parse.pokeValue(addr, typeField, allArgs[itp + 1])
            itp += 1

            if @err
                throw Format.toString("invalid value for option '%' (%)", oneArg, @err)
            if @countof(rest)
                throw Format.toString("invalid value for option '%'", oneArg)
        }
        else
        {
            throw Format.toString("unsupported type '%' for option '%'", typeField.flatName, oneArg)
        }

        // Set the associated 'isSet' boolean to true
        typeFieldIsSet := Reflection.getField(@typeof(result.isSet), oneArg)
        Debug.assert(typeFieldIsSet != null)
        addrIsSet := cast(*bool) &result.isSet
        addrIsSet += typeFieldIsSet.offset
        dref addrIsSet = true
    }

    return result
}
