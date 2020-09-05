struct(T) BB
{
    val: T = 666
}

struct(T) AA
{
    b: BB
}

#test
{
    var x: AA's32
    @assert(x.b.val == 666)
}