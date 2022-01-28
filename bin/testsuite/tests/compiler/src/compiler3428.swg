var G: s32

func toto()->closure()
{
    str := "10"
    a := 50
    b := 150
    var v: closure() = @|str,a,b|()
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
    a := toto()
    a()
    @assert(G == 200)
}