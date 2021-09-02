//#[Swag.PrintBc]
#test
{
    a := "string1"
    b := "string2"

    c := a orelse b
    @assert(c == "string1")

    a = null
    c = a orelse b
    @assert(c == "string2")
}