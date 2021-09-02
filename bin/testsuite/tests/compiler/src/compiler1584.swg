
#global testerror

#test
{
    enum Color
    {
        Red
        Green
        Blue
    }

    a := Color.Red

    #[Swag.Complete]
    switch a
    {
        case Color.Red: break
        case Color.Green: break
    }
}