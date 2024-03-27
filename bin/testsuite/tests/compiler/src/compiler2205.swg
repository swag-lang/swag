//#[Swag.PrintBc]
#test
{
    var arr = [0'u8, 1, 2, 3]
    var ptr = &arr[0]
    ptr += 1'u8
    @assert(ptr[0] == 1)

    let c = ptr[0]
    let x = ptr[c]
    @assert(x == 2)
}
