//#[Swag.printbc]
func titi()->[4] s64
{
    return @[1's64,2,3,4]
}

//#[Swag.printbc]
#test
{
    x := titi()
    @assert(x[0] == 1)
    @assert(x[1] == 2)
    @assert(x[2] == 3)
    @assert(x[3] == 4)
}