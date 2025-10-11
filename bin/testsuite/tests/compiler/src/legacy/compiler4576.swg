#test
{
    var x: [3] [2] s32
    @assert(#typeof(x).name == "[3][2] s32")

    x[0, 0] = 1
    x[0, 1] = 2
    @assert(x[0, 0] == 1)
    @assert(x[0, 1] == 2)
    //toto'([2] s32)()
}
