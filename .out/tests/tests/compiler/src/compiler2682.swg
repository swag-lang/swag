struct AA
{
    x: s64
}

impl AA
{
    #[Swag.implicit, Swag.complete]
    func opAffect(using self, p: s64)
    {
        x = p
    }
}

public func toto2682(v: AA)
{
    @assert(v.x == 2)
}

#[Swag.inline]
public func toto12682()
{
    toto2682(2's64)
}