struct(T) AA
{
    v:   T
}

func(T) inc(x: T)->T
{
    @assert(#typeof(T) == AA'f32)
    var ret: T
    ret.v = 666
    return ret
}

func(T) map(values: [..] T, cb: func(T)->T)
{
    for values do
        values[@index] = cb(values[@index])
}

#test
{
    var arr: [4] AA'f32
    map(cast() arr, &inc)
    @assert(arr[3].v == 666)
}
