func toto(x: [*] s32)
{
    let p = &x[0]
    #assert(#typeof(p) == #type [*] s32)
}
