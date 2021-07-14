func(T) inc(x: *T)
{
    dref x += 1
}

//#[Swag.PrintBc]
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
}