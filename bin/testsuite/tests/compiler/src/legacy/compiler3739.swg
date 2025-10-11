//#global testerror

//#[Swag.PrintBc, Swag.Optimize("", false)]
#[Swag.Safety("", false)]
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
