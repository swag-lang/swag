//#[Swag.PrintBc]
#test
{
    var arr   = [0'u8, 1, 2, 3]
    var arr1: [..] u8 = arr

    let c = arr1[1]
    arr1[c] = 1
    @assert(arr1[c] == 1)

    var ptr = @dataof(arr1)
    ptr[c] = 2
    @assert(arr1[c] == 2)

    var str = "ABCD"
    let y   = &str[c]
    @assert(y[1] == 'C''u8)
}
