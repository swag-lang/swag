#test
{
    var x:  const [2] s32 = [10, 20]
    let ptr = &x[0]
    @assert(#stringof(#typeof(ptr)) == "const [*] s32")
}

#test
{
    //const X: [2] s32 = [10, 20]
    //var ptr1 = &X[0]
}
