var G: s32

func toto()->closure()
{
    var str = "10"
    var a = 50
    var b = 150
    var v: closure() = closure|str,a,b|()
    {
        @assert(str == "10")
        @assert(a == 50)
        @assert(b == 150)
        G = a + b
    }

    return v
}

#test
{
    var a = toto()
    a()
    @assert(G == 200)
}