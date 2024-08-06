//#global skip
//#global testerror "Err0600"

func(T) max(a, b: T)
{
    //@print(@typeof(T).name)
}

#test
{
    max's32(1, 1)
    max(1, 1'f32)
}