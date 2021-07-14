//#[Swag.PrintBc]
#test
{
    arr := @[0'u8, 1, 2, 3]
    var arr1: [..] u8 = arr

    c := arr1[1]
    arr1[c] = 1
    @assert(arr1[c] == 1)

    ptr := @dataof(arr1)
    ptr[c] = 2
    @assert(arr1[c] == 2)

    str := "ABCD"
    y := &str[c]
    @assert(y[1] == "C"'u8)
}