//#[Swag.printbc]
#test
{
    x := "64"
    y := &x[0]
    @assert(y[0] == "6"'u8)
    @assert(y[1] == "4"'u8)
}