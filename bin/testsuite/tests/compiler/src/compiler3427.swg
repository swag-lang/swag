#test
{
    var str = "10"
    var a = 50
    var b = 150
    var v: closure() = closure|str,a,b|()
    {
        @assert(str == "10")
        @assert(a == 50)
        @assert(b == 150)
    }

    v()
}
