using Core

interface IGen
{
    func fn(self);
}

impl IGen for Core.String
{
    func impl fn(self)
    {
    }
}

#assert Reflection.hasInterface(@typeof(Core.String), IGen)
