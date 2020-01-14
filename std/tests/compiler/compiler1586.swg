#module compiler1586
#unittest error

#test
{
    a := "Color.Red"

    switch a
    {
        case "Color.Red":
        case "Color.Green":
        case "Color.Red":
    }
}