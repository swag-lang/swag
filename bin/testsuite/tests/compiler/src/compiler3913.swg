//#global skip

struct(T) A
{
    v:   s32
}

#test
{
    var a: A'(const [1] s32)
    var b: A'(#type [1] s32)
    #assert(#typeof(b).name == "A'([1] s32)")
}
