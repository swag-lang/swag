
#global testerror

#test
{
    enum Color : string
    {
        Red = "red"
        Green = "green"
        Blue = "blue"
    }

    a := Color.Red

    #[Swag.Complete]
    switch a
    {
        case Color.Red: break
        case Color.Green: break
    }
}