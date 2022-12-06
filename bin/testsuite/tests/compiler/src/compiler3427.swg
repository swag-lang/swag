#test
{
    str := "10"
    a := 50
    b := 150
    var v: closure() = closure|str,a,b|()
    {
        @assert(str == "10")
        @assert(a == 50)
        @assert(b == 150)
    }

    v()
}
