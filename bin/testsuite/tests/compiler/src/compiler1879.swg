#global testerror "Err0412"

func(T) AA()
{
    #assert(#typeof(T) != s32, "invalid type " ++ #typeof(T).name)
}

#test
{
    AA's32()
}