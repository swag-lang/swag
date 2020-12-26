func(T) inc(x: T)->T
{
    @assert(@typeof(T) == s32)
    return x * x
}

func(T) map(values: [..] T, cb: func(T)->T)
{
    loop values
        values[@index] = cb(values[@index])
}

#test
{
    arr := @[1,2,3,4]
    map(arr, &inc)
    @assert(arr[0] == 1)
    @assert(arr[1] == 4)
    @assert(arr[2] == 9)
    @assert(arr[3] == 16)
}