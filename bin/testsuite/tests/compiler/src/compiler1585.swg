
#global testerror "Err0436"

#test
{
    enum Color : string
    {
        Red = "red"
        Green = "green"
        Blue = "blue"
    }

    var a = Color.Red

    #[Swag.Complete]
    switch a
    {
        case Color.Red: break
        case Color.Green: break
    }
}