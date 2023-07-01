interface IIncrementable
{
	incr: func(self)
    decr: func(self)
    done: func(const self)->bool
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

    func impl decr(using self)
    {
        //@print("decr\n")
        val -= 1
    }

    func impl done(using const self)->bool
    {
        //@print("done\n")
        return val == 0
    }
}

impl TITI
{
    //#[Swag.PrintBc]
    func Loop(x: IIncrementable)
    {
        while !x.done()
            x.decr()
    }
}

//#[Swag.PrintBc]
#test
{
    var str: TITI
    TITI.Loop(&str)
    @assert(str.val == 0)
}