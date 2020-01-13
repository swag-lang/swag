#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    a := Color.Red

    #[swag.complete]
    switch a
    {
        case Color.Red:
        case Color.Green:
        case Color.Blue:
    }
}