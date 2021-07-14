enum CommandId
{
    Clear
    Transform
    DrawTriangles
    BindTexture
    SetClippingRect
    ResetClippingRect
    StartClippingRegion
    EndClippingRegion
    ResetClippingRegion
    SetClippingRegionMode
}

var G = 0

#[Swag.Inline]
func test()
{
    v := CommandId.StartClippingRegion
    switch v
    {
    case Clear:
        @assert(false)

    case Transform:
        @assert(false)

    case StartClippingRegion:
        fallthrough

    case SetClippingRegionMode:
        G = 1
    }
}

#test
{
    test()
}