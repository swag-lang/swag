//#[swag.printbc]
#test
{
    var arr: [4] s32 = @[10, 20, 30, 40]

    #[swag.relative(2)]
    var ptr: *s32 = &arr[0]

    @assert(@sizeof(ptr) == 2)
    @assert(ptr[0] == 10)

    ptr = ptr + 1
    @assert(ptr[0] == 20)

    ptr = ptr + 2
    @assert(ptr[0] == 40)
}