func toto(x: s32)
{
}

#test
{
    var v: [2] u8
    var a: const [..] u8 = v
    let ptr = &a[0]
    @assert(#stringof(#typeof(ptr)) == "const ^u8")
}