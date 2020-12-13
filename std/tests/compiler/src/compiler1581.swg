
#unittest error

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    a := 0//Color.Red

    #[swag.complete]
    switch a
    {
        /*case Color.Red:
        case Color.Green:
        case Color.Blue:*/
    }
}