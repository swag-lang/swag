func(T) inc(x: T)->T
{
    var y: T = 1
    @assert(@typeof(y) == s32)
    return x + y
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
    @assert(arr[0] == 2)
}