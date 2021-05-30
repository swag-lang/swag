#test
{
    var x: const [2] s32 = @[10, 20]
    ptr := &x[0]
    @assert(@stringof(ptr) == "const *s32")
}

#test
{
    //const X: [2] s32 = @[10, 20]
    //ptr1 := &X[0]
}