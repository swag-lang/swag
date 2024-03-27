interface IIncrementable
{
    func incr(self);
}

struct TITI
{
    val: s64 = 15
}

impl IIncrementable for TITI
{
    func impl incr(using self)
    {
        val += 1
    }
}

func Loop(x: IIncrementable)
{
}

//#[Swag.PrintBc]
#test
{
    var str: TITI
    Loop(str)
}
