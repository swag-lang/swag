struct IntPoint
{
    Y: s32
}

impl IntPoint
{
    #[Swag.NoInline]
    mtd opEquals(other: Self)->bool
    {
        return Y == other.Y
    }
}

func GetBottomPt()
{
    var pp: IntPoint = undefined
    while pp != pp {}
}
