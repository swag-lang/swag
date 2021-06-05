//#[Swag.printbc]
#[Swag.inline]
public func toto2927()
{
    cpt := 0
    loop 1..10
    {
        cpt += 1
    }

    @assert(cpt == 10)
}