struct(T) A {}

impl A
{
    mtd opIndex(i: u64)->s32
    {
        #assert(#typeof(T).fullname == "s32")
        return 0
    }
}

#test
{
    var x: A's32
    var xx = x[0]

    alias S32 = s32
    var y: A'S32
    var yy = y[0]

    var xx1 = x[0]
}
