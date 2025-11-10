enum DockStyle
{
    None
    Top
    Left
    Right
    Bottom
    Center
}

func toto(x: DockStyle)
{
    for 2
    {
        switch x
        {
        case Left:
            break
        case Right:
            break
        case Top:
            @print("Top")
        case Bottom:
            @print("Bottom")
        case Center:
            @print("Center")
        }
    }
}

#test
{
    toto(.None)
}
