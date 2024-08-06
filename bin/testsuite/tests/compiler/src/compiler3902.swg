#global testerror "Err0609"

func(T) toto(x: T)
{
}

#test
{
    toto'([2] s32)([1, 2])
    //toto'(f32)(true)
}