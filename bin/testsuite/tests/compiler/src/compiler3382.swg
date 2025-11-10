#test
{
    var fct: func(s32)
    fct = func(x)
    {
        #assert(#typeof(x) == s32)
    }

    var fct1: func(s32, bool)
    fct1 = func(x, y)
    {
        #assert(#typeof(x) == s32)
        #assert(#typeof(y) == bool)
    }
}
