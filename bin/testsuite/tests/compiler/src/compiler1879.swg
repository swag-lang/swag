#global testerror "Err0426"

func(T) AA()
{
    #assert(@typeof(T) != s32, "invalid type " ++ @typeof(T).name)
}

#test
{
    AA's32()
}