var G: s32

func toto()->closure()
{
    let str = "10"
    let a = 50
    let b = 150
    let v: closure() = closure|str,a,b|()
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
    let a = toto()
    a()
    @assert(G == 200)
}