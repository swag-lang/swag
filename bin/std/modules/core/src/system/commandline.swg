#global export
#global namespace CommandLine

attr ArgParams(nameAlias: string = null)

struct ParseOptions
{
}

// The 'IsSet' generic struct is a mirror of the user input struct.
//
// Each field has the same name as the original one, but with a 'bool' type.
//
// It will indicate, after the command line parsing, that the corresponding argument has been
// specified or not by the user.
struct(T) IsSet
{
    #ast
    {
        var str = StrConv.StringBuilder{}
        let typeof = @typeof(T)
        visit f: typeof.fields
            str.appendFormat("%: bool\n", f.name)
        return str.toString()
    }
}

// Result of the [parse] function
struct(T) Result
{
    isSet: IsSet
}

// Returns true if 'oneArg' is a valid option (starting with a delimiter)
func isOption(oneArg: string, _options: ParseOptions)->bool
{
    if @countof(oneArg) == 0
        return false
    return oneArg[0] == `-`
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

        var split = Tokenize.split(oneArg, [`:`'rune, `=`'rune], 1)
        if split.count == 2
        {
            allArgs.add(split[0])
            let other = Tokenize.eatSpaces(split[1])
            if @countof(other)
                allArgs.add(other)
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

        // Test name typealias
        let value = Reflection.getAttributeValue(it, ArgParams, "nameAlias")
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
            throw cast(string) Format.toString("missing value for option '%'", oneArg)
    }

    var result:  retval

    // First pass
    var allArgs = splitArguments(args, options)

    // Parse all
    let typeof = @typeof(T)
    let maxItp = @countof(allArgs)
    for var itp = 0; itp < maxItp; itp += 1
    {
        var oneArg = allArgs[itp]
        if !isOption(oneArg, options)
            throw cast(string) Format.toString("invalid option '%'", oneArg)

        oneArg = Tokenize.eatCount(oneArg, 1)

        let typeParam = getField(typeof, oneArg)
        if !typeParam
            throw cast(string) Format.toString("unknown option '%'", oneArg)

        let typeField = typeParam.pointedType

        var addr = cast(^u8) st
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
            let rest = catch StrConv.pokeValue(addr, typeField, allArgs[itp + 1])
            itp += 1

            if @err() != null
                throw cast(string) Format.toString("invalid value for option '%' (%)", oneArg, @errmsg)
            if @countof(rest)
                throw cast(string) Format.toString("invalid value for option '%'", oneArg)
        }

        // Dynamic array of values
        elif Reflection.isStruct(typeField)
        {
            let typeStruct = cast(const *Swag.TypeInfoStruct) typeField
            if typeStruct.fromGeneric == @typeof(Core.Array)
            {
                try checkNext()

                // If it's an array of strings, we will need to add string delimiters
                // to each value
                var isString = false
                if typeStruct.generics[0].pointedType == Core.String
                    isString = true

                // Make one single string with all values, for pokeValue
                var collect: String
                while itp + 1 != maxItp and !isOption(allArgs[itp + 1], options)
                {
                    var other = allArgs[itp + 1]
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
                if @err() != null
                    throw cast(string) Format.toString("invalid value for option '%' (%)", oneArg, @errmsg)
            }
            else
            {
                throw cast(string) Format.toString("unsupported type '%' for option '%'", typeField.name, oneArg)
            }
        }
        else
        {
            throw cast(string) Format.toString("unsupported type '%' for option '%'", typeField.name, oneArg)
        }

        // Set the associated 'isSet' boolean to true
        let typeFieldIsSet = Reflection.getField(@typeof(result.isSet), typeParam.name)
        Debug.assert(typeFieldIsSet != null)
        var addrIsSet = cast(^bool) &result.isSet
        addrIsSet += typeFieldIsSet.offset
        dref addrIsSet = true
    }

    return result
}
