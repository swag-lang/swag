//#[Swag.PrintBc]
#test
{
    var a = "toto"

    var start = 1'u32
    var end = 2'u32
    var c = a[start..end]
    @assert(c == "ot")
}