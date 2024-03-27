#test
{
    let str = "10"
    let v:  closure() = closure|str|()
    {
        @assert(str == "10")
    }

    v()
}
