#[Swag.Inline]
public func x3193()
{
    var G = 0

    #[Swag.Mixin]
    func xx()
    {
        G = 5
    }

    xx()
    @assert(G == 5)
}

//#[Swag.PrintBc]
#test
{
    x3193()
}
