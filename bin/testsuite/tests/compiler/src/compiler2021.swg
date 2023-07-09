func(T) inc(x: T) => x + 1

func(T) map(values: [..] T, cb: func(T)->T)
{
    loop values
        values[@index] = cb(values[@index])
}

#test
{
    var arr = [1,2,3,4]
    //map(arr, &inc's32)
    map(acast arr, func(x) => x * x)
    @assert(arr[0] == 1)
    @assert(arr[1] == 4)
    @assert(arr[2] == 9)
    @assert(arr[3] == 16)
}