#test
{
    var buffer: [10] u8
    var ptr: *u8 = buffer
    ptr[0] = 66
    @assert(buffer[0] == 66)
    @assert(dref ptr == 66)
}