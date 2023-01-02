//#global testerror

//#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    x := 1

    #[Swag.Complete]
    #[Swag.Safety("range", false)]
    switch x
    {
    case 0: @print("0")
    }
}