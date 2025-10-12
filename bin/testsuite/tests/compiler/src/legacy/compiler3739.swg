//#global testerror

//#[Swag.PrintBc, Swag.Optimize("", false)]
#[Swag.Safety(.All, "", false)]
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
