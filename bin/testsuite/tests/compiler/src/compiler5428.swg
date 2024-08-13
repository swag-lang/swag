//#global skip
//#global testerror "Err0506"

func(T) max(a, b: T, c: string)
{
    @assert(@typeof(T).name == c)
}

#test
{
    max's16(1, 1, "s16")
    max(1, 1, "s32")
}