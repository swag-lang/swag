//#global testerror "Err0004"

func(T) add(x, y: T)->T
{
    return x + y
}

#test
{
    var v  = add(1'u8, 2)
    var v1 = add(1'u16, 2)
    var v2 = add(1, 2)
}
