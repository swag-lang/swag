//#global testerror

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x = 1

    //#[Swag.Complete]
    //#[Swag.Safety("switch", false)]
    switch x
    {
    case 0: @print("0")
    }
}