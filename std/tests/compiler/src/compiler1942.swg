struct(T) BB
{
    val: T = 666
}

struct(T) AA
{
    b: BB
}

func titi(st: AA)
{
    @assert(st.b.val == 666)
}

#test
{
    var x: AA's64
    @assert(x.b.val == 666)
    titi(x)

    var y: AA'f64
    @assert(y.b.val == 666)
    titi(y)
}