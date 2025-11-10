//#[Swag.PrintBc]
#test
{
    var a = "string1"
    let b = "string2"

    var c = a orelse b
    @assert(c == "string1")

    a = null
    c = a orelse b
    @assert(c == "string2")
}
