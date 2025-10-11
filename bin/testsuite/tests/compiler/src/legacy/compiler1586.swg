#global testerror "Err0016"

#test
{
    var a = "Color.Red"

    switch a
    {
    case "Color.Red":
        break
    case "Color.Green":
        break
    case "Color.Red":
        break
    }
}
