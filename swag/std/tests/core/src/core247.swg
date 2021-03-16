using core, strconv

#ast
{
    var str: StringBuilder
    str.appendString("const tt: [] s64 = @[")

    loop 10
    {
        if @index str.appendString(",")
        str.appendFormat("%", @index)
    }

    str.appendString("]")
    return cast(string) str.toString()
}

#placeholder tt
#test
{
    visit s: tt
    {
        @print(s)
        @print("\n")
    }
}