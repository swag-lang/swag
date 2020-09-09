interface ITF
{
    toto: func(self)
}

struct AA
{

}

impl ITF for AA
{
    func toto(self)
    {
    }
}

func(T) gen(x: T)
{
    itf := @mkinterface(x, @typeof(T), ITF)
    if itf != null
        itf.toto()
}

#test
{
    var x: AA
    gen'AA(x)
}