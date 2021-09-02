interface IIncrementable
{
	incr: func(self)
    //decr: func(self)
    //done: func(self)->bool
}

struct TITI
{
    val: s64 = 15
}

impl IIncrementable for TITI
{
    func incr(using self)
    {
        val += 1
    }

    /*func decr(using self)
    {
        val -= 1
    }

    func done(using self)->bool
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