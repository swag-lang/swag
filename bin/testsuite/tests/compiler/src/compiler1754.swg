var x: string = "64"

//#[Swag.PrintBc]
#test
{
    let y = &x[0]
    @assert(y[0] == '6''u8)
    @assert(y[1] == '4''u8)
}