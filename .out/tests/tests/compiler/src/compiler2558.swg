struct AA
{

}

impl AA
{
    func opCast(self)->s32
    {
        return 0
    }
}

func(T) toto(x: T)
{
}

#test
{
    var x: AA
    toto(x)
    toto(0's32)
    toto(x)
}