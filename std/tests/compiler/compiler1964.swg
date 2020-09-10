struct A
{
    x: s64 = 555
}

impl A
{
    func titi(using self)
    {
        @assert(x == 555)
    }

    func toto(using self)
    {
        titi()
    }
}

#test
{
    var tt: A
    tt.toto()
}