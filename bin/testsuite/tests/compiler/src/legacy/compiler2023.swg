func(T) inc(x: T) => x * x

func(T) map(values: [..] T, cb: func(T)->T)
{
    for values do
        values[@index] = cb(values[@index])
}

#test
{
    var arr = [1, 2, 3, 4]
    map(cast() arr, &inc)
    @assert(arr[0] == 1)
    @assert(arr[1] == 4)
    @assert(arr[2] == 9)
    @assert(arr[3] == 16)
}
