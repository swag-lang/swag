#test
{
    func toto(x: func(string))
    {
        x("64")
    }

    toto(@(x) { @assert(x == "64"); })
}