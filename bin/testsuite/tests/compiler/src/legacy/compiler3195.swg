#[Swag.Inline]
public func x3195()
{
    #[Swag.Mixin]
    {
        #[Swag.Safety(.All, "", false)]
        {
            func xx()
            {
                @assert(true)
            }
        }
    }

    xx()
}

//#[Swag.PrintBc]
#test
{
    x3195()
}
