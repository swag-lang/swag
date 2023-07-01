using Core

interface IGen
{
    fn: func(self)
}

impl IGen for Core.String
{
    func impl fn(self)
    {

    }
}

#assert Reflection.hasInterface(@typeof(Core.String), IGen)