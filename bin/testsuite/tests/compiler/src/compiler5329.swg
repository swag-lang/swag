#global skip
func(T) toto(a: {x: T})->{x: T}
{
    return {x: 0}
}

#test
{
    let a = &toto's32
    let b = a({0's32})
    #print(#typeof(a).name)
    #assert(#typeof(a).name == "func'(s32)(const) &{x:s32})->{x:s32}"
}