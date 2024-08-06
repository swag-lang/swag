#global testerror "Err0474"

func(T) AA()
{
    #assert(@typeof(T) != s32, "invalid type " ++ @typeof(T).name)
}

#test
{
    AA's32()
}