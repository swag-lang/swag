struct A
{
    x:   s64 = 555
}

impl A
{
    func titi(me)
    {
        @assert(me.x == 555)
    }

    func toto(me)
    {
        me.titi()
    }
}

#test
{
    var tt: A
    tt.toto()
}
