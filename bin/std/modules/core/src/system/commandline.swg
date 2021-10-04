#global export
#global namespace CommandLine

attr ArgParams(nameAlias: string = null);

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

func getField(typeStruct: const *Swag.TypeInfoStruct, name: string)->const *Swag.TypeValue
{
    visit it: typeStruct.fields
    {
        // Test normal name (field)
        if it.name == name
            return it

        // Test name alias
        (found, val) := Reflection.getAttributeValue(it, "Core.CommandLine.ArgParams", "nameAlias")
        if found and cast(string) val == name
            return it
    }

    return null
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

        typeParam := getField(typeof, oneArg)
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

        // All types supported by `Parse.pokeValue`
        else if typeField == @typeof(Core.String) or
                Reflection.isInteger(typeField) or
                Reflection.isFloat(typeField) or
                Reflection.isBool(typeField) or
                Reflection.isRune(typeField) or
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

        // Dynamic array of values
        else if Utf8.startsWith(typeField.name, "Core.Array")
        {
            try checkNext()

            // If it's an array of strings, we will need to add string delimiters
            // to each value
            typeStruct := cast(const *Swag.TypeInfoStruct) typeField
            isString := false
            if typeStruct.generics[0].pointedType == Core.String
                isString = true

            // Make one single string with all values, for pokeValue
            var collect: String
            while itp + 1 != maxItp and !isOption(allArgs[itp + 1], options)
            {
                other := allArgs[itp + 1]
                if isString
                {
                    other = Tokenize.eatQuotes(other)
                    collect.append("\"")
                }

                collect.append(other)

                if isString
                    collect.append("\"")

                collect.append(" ")
                itp += 1
            }

            discard catch Parse.pokeValue(addr, typeField, collect.toString())
            if @err
                throw Format.toString("invalid value for option '%' (%)", oneArg, @err)
        }

        else
        {
            throw Format.toString("unsupported type '%' for option '%'", typeField.flatName, oneArg)
        }

        // Set the associated 'isSet' boolean to true
        typeFieldIsSet := Reflection.getField(@typeof(result.isSet), typeParam.name)
        Debug.assert(typeFieldIsSet != null)
        addrIsSet := cast(*bool) &result.isSet
        addrIsSet += typeFieldIsSet.offset
        dref addrIsSet = true
    }

    return result
}
