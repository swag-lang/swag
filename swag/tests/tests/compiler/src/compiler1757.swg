//#[swag.printbc]
#test
{
    a := "string1"
    b := "string2"

    c := a ?? b
    @assert(c == "string1")

    a = null
    c = a ?? b
    @assert(c == "string2")
}