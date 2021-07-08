struct IntPoint
{
    Y: s32
}

impl IntPoint
{
    method opEquals(other: Self)->bool
    {
        return Y == other.Y
    }
}

func GetBottomPt()
{
    var pp: IntPoint = ?
    while pp == pp {}
}
