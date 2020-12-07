#module "compiler1584"
#unittest error

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
        case Color.Red: break
        case Color.Green: break
    }
}