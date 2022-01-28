#test
{
    str := "10"
    var v: closure() = @|str|()
    {
        @assert(str == "10")
    }

    v()
}
