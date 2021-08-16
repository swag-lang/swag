//#[Swag.PrintBc]
#test
{
    var a, b: [16] u8
    @memcpy(a, b, 0)
    @memcpy(a, b, 1)
    @memcpy(a, b, 2)
    @memcpy(a, b, 4)
    @memcpy(a, b, 8)
}