using Core

interface IGen
{
    func fn(me);
}

impl IGen for Core.String
{
    func impl fn(me) {}
}

#assert(Reflection.hasInterface(#typeof(Core.String), IGen))
