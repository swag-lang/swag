struct A { x, y: s32 }

#test
{
    var arr:  [4] A
    let slice = arr[ to ]
    foreach v in slice
    {
        v = A{1, 2}
    }

    @assert(arr[0].x == 1)
    @assert(arr[0].y == 2)
}
