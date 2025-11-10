//#[Swag.PrintBc]
func toString()->const [..] u8
{
    return [0, 1, 2, 3]
}

#test
{
    let x = toString()
    @assert(x[0] == 0)
    @assert(x[1] == 1)
    @assert(x[2] == 2)
    @assert(x[3] == 3)
}
