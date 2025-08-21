struct A
{
    x: s64 = 555
}

impl A
{
    func titi(using me)
    {
        @assert(x == 555)
    }

    func toto(using me)
    {
        titi()
    }
}

#test
{
    var tt: A
    tt.toto()
}