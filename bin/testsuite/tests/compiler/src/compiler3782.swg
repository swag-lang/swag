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
        case Top:
            break
        case Right:
            break
        case Left:
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
