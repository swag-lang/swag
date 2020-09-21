func(T) inc(x: *T)
{
    deref x += 1
}

//#[swag.printbc]
func(T) map(values: [..] T, cb: func(*T))
{
    visit *it: values
        cb(it)
    loop values
        cb(&values[@index])
}

#test
{
    var arr: [4] f32
    map(arr, &inc'f32)
    @assert(arr[0] == 2)
    @assert(arr[3] == 2)
}