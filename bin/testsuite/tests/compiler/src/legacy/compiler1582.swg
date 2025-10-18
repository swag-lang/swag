#global testerror "Err0193"

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    var a = Color.Red

    #[Swag.Complete]
    switch a
    {
    case Color.Red:
        break
    case Color.Red:
        break
    case Color.Green:
        break
    case Color.Blue:
        break
    }
}
