//#[swag.printbc]
#test
{
    arr := @[0'u8, 1, 2, 3]
    ptr := &arr[0]
    ptr += 1'u8
    @assert(ptr[0] == 1)

    c := ptr[0]
    x := ptr[c]
    @assert(x == 2)
}