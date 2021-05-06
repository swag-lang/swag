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

#assert reflection.hasInterface(@typeof(core.String), IGen)