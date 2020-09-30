//#[swag.printbc]
func set(ptr: [..] u16, val: u16)
{
    //@print(cast(s64) @countof(ptr))
    ptr[0] = val
}

//#[swag.printbc]
#test
{
    var buffer: [64] u16

    set(buffer, 555)
    @assert(buffer[0] == 555)

    var a: [..] u16 = @mkslice(&buffer[0], 64)
    set(a, 666)
    @assert(buffer[0] == 666)

    //set({&buffer[0], 64}, 777)
    //@assert(buffer[0] == 777)
}