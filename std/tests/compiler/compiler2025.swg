func(T) inc(x: T)->T
{
    @print(@typeof(T).name)
    return x * x
}

func(T) inc1(x: T)->T
{
    var y: T = 1
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
    //map(arr, &inc1)
}