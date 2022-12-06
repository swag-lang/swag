#test
{
    str := "10"
    var v: closure() = closure|str|()
    {
        @assert(str == "10")
    }

    v()
}
