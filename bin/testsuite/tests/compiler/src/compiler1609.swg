//#[Swag.PrintBc]
#test
{
    var buffer: [10] u8
    var length: u32

    buffer[0] = 65
    buffer[1] = 66
    buffer[2] = 0
    var str: string = @mkstring(&buffer[0], 2)
    @assert(str == "AB")
}