#global testerror "Err0063"

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    var a = 0//Color.Red

    #[Swag.Complete]
    switch a
    {
        /*case Color.Red:
        case Color.Green:
        case Color.Blue:*/
    }
}