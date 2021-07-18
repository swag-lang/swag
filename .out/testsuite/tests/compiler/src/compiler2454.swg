#global skip // no more relative pointers (18 july 2021)
//#[Swag.PrintBc]
#test
{
    var arr: [4] s32 = @[10, 20, 30, 40]
    var ptr: *~32 s32 = &arr[0]

    @assert(@sizeof(ptr) == 4)
    @assert(ptr[0] == 10)
    @assert(ptr[1] == 20)
    @assert(ptr[2] == 30)
    @assert(ptr[3] == 40)
}