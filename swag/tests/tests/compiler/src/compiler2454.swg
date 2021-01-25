//#[swag.printbc]
#test
{
    var arr: [4] s32 = @[10, 20, 30, 40]

    #[swag.relative(4)]
    var ptr: *s32 = &arr[0]

    @assert(@sizeof(ptr) == 4)
    @assert(ptr[0] == 10)
    @assert(ptr[1] == 20)
    @assert(ptr[2] == 30)
    @assert(ptr[3] == 40)
}