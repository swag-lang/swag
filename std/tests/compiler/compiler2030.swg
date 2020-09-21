func(T) inc(x: *T)
{
    :x += 1
}

func(T) dec(x: *T)
{
    :x -= 1
}

//#[swag.printbc]
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
    map(arr, &dec)
    @assert(arr[0] == 0)
    @assert(arr[3] == 0)
}