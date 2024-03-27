#[Swag.Inline]
public func x3194()
{
    #[Swag.Mixin, Swag.Safety("", false)]
    {
        func xx()
        {
            @assert(true)
        }
    }

    xx()
}

//#[Swag.PrintBc]
#test
{
    x3194()
}