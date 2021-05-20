using core

interface IGen
{
    fn: func(self)
}

impl IGen for core.String
{
    func fn(self)
    {

    }
}

#assert Reflection.hasInterface(@typeof(core.String), IGen)