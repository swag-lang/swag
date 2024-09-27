#test
{
    func toto(x: nn func(bool,string))
    {
        x(true,"64")
    }

    toto(func(x,y) { @assert(y == "64"); })
}