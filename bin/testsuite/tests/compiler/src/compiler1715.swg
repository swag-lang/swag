#test
{
    func toto(x: nn func(string))
    {
        x("64")
    }

    toto(func(x) { @assert(x == "64"); })
}