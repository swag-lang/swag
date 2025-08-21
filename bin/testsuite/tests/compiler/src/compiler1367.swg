interface IIncrementable
{
	func incr(me);
    func decr(me);
    func done(const me)->bool;
}

struct TITI
{
    val: s64 = 15
}

impl IIncrementable for TITI
{
    func impl incr(using me)
    {
        val += 1
    }

    func impl decr(using me)
    {
        //@print("decr\n")
        val -= 1
    }

    func impl done(using const me)->bool
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
        while !x.done():
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