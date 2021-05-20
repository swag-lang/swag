func(T) inc(x: *T)
{
    dref x += 1
}

//#[Swag.printbc]
func(T) map(values: [..] T, cb: func(*T))
{
    loop values
        cb(&values[@index])
}

#test
{
    var arr: [4] f32
    map(arr, &inc)
    @assert(arr[0] == 1)
    @assert(arr[3] == 1)

    var arr1: [4] s64
    map(arr1, &inc)
    @assert(arr1[0] == 1)
    @assert(arr1[3] == 1)
}