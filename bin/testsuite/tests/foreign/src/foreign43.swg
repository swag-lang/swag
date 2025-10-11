using Swag

#[AttrUsage(AttributeUsage.Struct)]
public attr Final()

#[Final]
public struct TT43 { x, y: s32 }

public impl TT43
{
    const Zero: TT43{0, 0}
}
