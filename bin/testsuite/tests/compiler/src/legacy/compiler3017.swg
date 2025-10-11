struct IntPoint
{
    Y:   s32
}

impl IntPoint
{
    #[Swag.NoInline]
    mtd opEquals(other: IntPoint)->bool
    {
        return me.Y == other.Y
    }
}

func GetBottomPt()
{
    var pp: IntPoint = undefined
    while pp != pp
    {
    }
}
