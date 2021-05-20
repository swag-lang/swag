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
    func incr(using self)
    {
        val += 1
    }

    func decr(using self)
    {
        val -= 1
    }

    func done(using const self)->bool
    {
        return val == 0
    }
}

//#[Swag.printbc]
func Loop(x: *IIncrementable)
{
    while !x.done()
        x.decr()
}

//#[Swag.printbc]
#test
{
    var str: TITI
    Loop(&str)
    @assert(str.val == 0)
}