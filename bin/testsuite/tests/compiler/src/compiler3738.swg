#global testerror "Saf0019"

//#[Swag.PrintBc, Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = 1

    #[Swag.Complete]
    switch x
    {
    case 0: @print("0")
    }
}