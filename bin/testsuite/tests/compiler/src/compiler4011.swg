#global testerror "Err0609"

func(T) toto(x: s32, y: T)
{
}

#test
{
    toto(0.0'f32, 0.0'f32)
}