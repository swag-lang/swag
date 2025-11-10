interface IIncrementable
{
    func incr(me);
    //decr: func(me)
    //done: func(me)->bool
}

struct TITI
{
    val: s64 = 15
}

impl IIncrementable for TITI
{
    func impl incr(me)
    {
        me.val += 1
    }

    /*func decr(me)
    {
        val -= 1
    }

    func done(me)->bool
    {
        return val == 0
    } */
}

func Loop(x: IIncrementable)
{
    //while !x.done()
    //   x.decr()
}

//#[Swag.PrintBc]
#test
{
    var str: TITI
    Loop(str)
}
