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
    func impl incr(me)
    {
        me.val += 1
    }

    func impl decr(me)
    {
        //@print("decr\n")
        me.val -= 1
    }

    func impl done(const me)->bool
    {
        //@print("done\n")
        return me.val == 0
    }
}

impl TITI
{
    //#[Swag.PrintBc]
    func Loop(x: IIncrementable)
    {
        while !x.done() do
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
