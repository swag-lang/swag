using Core, Core.Utf8, Core.Tokenize

//#[Swag.PrintBc]
#test
{
    var x: String = "This is a list of words, with: a bit of punctuation\tand a tab character."

    var sep = [' ''rune, ",", ".", ":", "\t"]
    var arr = x.splitAny(sep)

    foreach v in arr
    {
        //@print(v)
        //@print("\n")
    }

    @assert(arr.count == 15)
    @assert(arr[0] == "This")
    @assert(arr[14] == "character")
}

#test
{
    var x:  String = "This is a list of words"
    var arr = x.splitAny([' ''rune], 1)
    @assert(arr.count == 2)
    @assert(arr[0] == "This")
    @assert(arr[1] == "is a list of words")
}
