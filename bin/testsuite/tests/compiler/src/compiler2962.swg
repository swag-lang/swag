public struct V2962
{
    x, y: s32
}

public impl V2962
{
    func toto(using self, angle: f32 = 0, pos1: V2962 = @{1, 1})
    {
    }
}

#[Swag.Inline]
public func titi2962()
{
    var v: V2962
    v.toto()
}