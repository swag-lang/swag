struct AA
{
    x: s32
}

interface ITF
{
    cb: func(self)
}

impl ITF for AA
{
    func cb(using self)
    {

    }
}

#test
{
    @print(666's64)
    //var x: AA
    //tt := @mkinterface(&x, @typeof(x), @typeof(ITF))
}