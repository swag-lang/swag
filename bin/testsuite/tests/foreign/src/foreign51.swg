public struct AA50 { x, y: s32 }

#[Swag.Inline]
public func toto50()
{
    var a: AA50
    with a
    {
        .x = 0
        .y = 0
    }
}
