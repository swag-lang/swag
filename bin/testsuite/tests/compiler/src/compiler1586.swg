#global testerror "Err0527"

#test
{
    var a = "Color.Red"

    switch a
    {
        case "Color.Red": break
        case "Color.Green": break
        case "Color.Red": break
    }
}