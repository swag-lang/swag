//#global testerror

//#[Swag.PrintBc, Swag.Optimize("", false)]
#test
{
    let x = 1

    //#[Swag.Complete]
    //#[Swag.Safety("switch", false)]
    switch x
    {
    case 0: @print("0")
    }
}