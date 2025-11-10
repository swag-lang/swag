#global skip
#test
{
    var a: [4] s32 = [1, 2, 3, 4]
    a[1..2] = [3, 2]
    @assert(a[1] == 3)
    @assert(a[2] == 2)

    let ptr = &a[0]
    ptr[0..1] = ptr[2..3]
    @assert(a[0] == 2)
    @assert(a[1] == 4)
}