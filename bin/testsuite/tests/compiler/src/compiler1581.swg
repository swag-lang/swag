
#global testerror

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    a := 0//Color.Red

    #[Swag.Complete]
    switch a
    {
        /*case Color.Red:
        case Color.Green:
        case Color.Blue:*/
    }
}