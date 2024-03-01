#global skip
//#global testerror "Err0613"

func(T) max(a, b: T)
{
    @print(@typeof(T).name)
}

#test
{
    max's16(1, 1)
    max(1, 1)
}