func concrete()->s32
{
    return 666
}

func concrete1()->f32
{
    return 666
}

//#[Swag.PrintBc]
func(T) toto(x: func()->T)->T
{
    return x()
}

#test
{
    let lambda = &concrete
    let result = toto(lambda)
    @assert(result == 666)

    let lambda1 = &concrete1
    let result1 = toto(lambda1)
    @assert(result1 == 666.0)
}
