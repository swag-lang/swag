#test
{
    var buffer: [..] u8 = [0, 0]
    var ptr: *u8 = buffer
    ptr[0] = 66
    @assert(buffer[0] == 66)
    @assert(deref ptr == 66)
}