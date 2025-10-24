//#global testerror

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#[Swag.Safety(.All, .All, false)]
#test
{
    let x = 1

    //#[Swag.Complete]
    switch x
    {
    case 0:
        @print("0")
    }
}
