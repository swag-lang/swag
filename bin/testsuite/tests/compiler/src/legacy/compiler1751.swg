//#[Swag.PrintBc]
#test
{
    var x = "64"
    let y = &x[0]
    @assert(y[0] == '6''u8)
    @assert(y[1] == '4''u8)
}
