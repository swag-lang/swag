#global testerror "Err0012"

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