public struct AA2682
{
    x:   s64
}

public impl AA2682
{
    #[Swag.Implicit, Swag.Complete]
    func opAffect(me, p: s64)
    {
        me.x = p
    }
}

public func toto2682(v: AA2682)
{
    @assert(v.x == 2)
}

#[Swag.Inline]
public func toto12682()
{
    toto2682(2's64)
}
