#global export
#global namespace CommandLine

attr ArgParams(nameAlias: string = null)

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
}

// Returns true if 'oneArg' is a valid option (starting with a delimiter)
func isOption(oneArg: string, _options: ParseOptions)->bool
{
    if @countof(oneArg) == 0
        return false
    return oneArg[0] == "-"'u8
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

        split := Tokenize.split(oneArg, [":"'rune, "="'rune], 1)
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
        value := Reflection.getAttributeValue(it, ArgParams, "nameAlias")
        if @dataof(value) and Reflection.isString(@kindof(value)) and cast(string) value == name
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
            throw Format.toString("invalid option '%'", oneArg)

        oneArg = Tokenize.eatCount(oneArg, 1)

        typeParam := getField(typeof, oneArg)
        if !typeParam
            throw Format.toString("unknown option '%'", oneArg)

        typeField := typeParam.pointedType

        addr := cast(^u8) st
        addr += typeParam.offset

        // Bool: Result is true if the option does not have an associated value
        if Reflection.isBool(typeField) and
           (itp + 1 == maxItp or isOption(allArgs[itp + 1], options))
        {
            dref cast(*bool) addr = true
        }

        // All types supported by 'Parse.pokeValue'
        elif typeField == @typeof(Core.String) or
                Reflection.isInteger(typeField) or
                Reflection.isFloat(typeField) or
                Reflection.isBool(typeField) or
                Reflection.isRune(typeField) or
                Reflection.isEnum(typeField)
        {
            try checkNext()
            rest := catch StrConv.pokeValue(addr, typeField, allArgs[itp + 1])
            itp += 1

            if @errmsg
                throw Format.toString("invalid value for option '%' (%)", oneArg, @errmsg)
            if @countof(rest)
                throw Format.toString("invalid value for option '%'", oneArg)
        }

        // Dynamic array of values
        elif Reflection.isStruct(typeField)
        {
            typeStruct := cast(const *Swag.TypeInfoStruct) typeField
            if typeStruct.fromGeneric == @typeof(Core.Array)
            {
                try checkNext()

                // If it's an array of strings, we will need to add string delimiters
                // to each value
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

                discard catch StrConv.pokeValue(addr, typeField, collect.toString())
                if @errmsg
                    throw Format.toString("invalid value for option '%' (%)", oneArg, @errmsg)
            }
            else
            {
                throw Format.toString("unsupported type '%' for option '%'", typeField.name, oneArg)
            }
        }
        else
        {
            throw Format.toString("unsupported type '%' for option '%'", typeField.name, oneArg)
        }

        // Set the associated 'isSet' boolean to true
        typeFieldIsSet := Reflection.getField(@typeof(result.isSet), typeParam.name)
        Debug.assert(typeFieldIsSet != null)
        addrIsSet := cast(^bool) &result.isSet
        addrIsSet += typeFieldIsSet.offset
        dref addrIsSet = true
    }

    return result
}
