#module "compiler1586"
#unittest error

#test
{
    a := "Color.Red"

    switch a
    {
        case "Color.Red": break
        case "Color.Green": break
        case "Color.Red": break
    }
}