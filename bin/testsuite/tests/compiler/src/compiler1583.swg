#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    let a = Color.Red

    #[Swag.Complete]
    switch a
    {
        case Color.Red: break
        case Color.Green: break
        case Color.Blue: break
    }
}