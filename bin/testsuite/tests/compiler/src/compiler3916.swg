#test
{
    var x: [2] const [..] s32
    @assert(#typeof(x).name == "[2] const [..] s32")

    x[0] = [1, 2]
    x[1] = [3, 4]
    @assert(x[0, 0] == 1)
    @assert(x[0, 1] == 2)
    @assert(x[1, 0] == 3)
    @assert(x[1, 1] == 4)
}
