//#[Swag.PrintBc]
#[Swag.Inline]
public func toto2927()
{
    cpt := 0
    loop 1..10
    {
        cpt += 1
    }

    @assert(cpt == 10)
}