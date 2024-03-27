using Core, Core.Utf8, Core.Tokenize

//#[Swag.PrintBc]
#test
{
    var x:  String = "    toto titi  指事字   "
    var arr = x.split(` `'rune)
    @assert(arr.count == 3)
    @assert(arr[0] == "toto")
    @assert(arr[1] == "titi")
    @assert(arr[2] == "指事字")
}
