//#global testerror

//#[Swag.PrintBc, Swag.Optim("", false)]
#[Swag.Safety("", false)]
#test
{
    x := 1

    //#[Swag.Complete]
    switch x
    {
    case 0: @print("0")
    }
}