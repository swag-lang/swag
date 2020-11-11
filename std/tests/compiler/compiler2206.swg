//#[swag.printbc]
#test
{
    arr := @[0'u8, 1, 2, 3]
    var arr1: [..] u8 = arr

    c := arr1[1]
    arr1[c] = 1
    @assert(arr1[c] == 1)
}