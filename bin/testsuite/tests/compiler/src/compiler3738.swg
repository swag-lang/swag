#global testerror "Saf0028"

//#[Swag.PrintBc, Swag.Optim("", false)]
#[Swag.Safety("", true)]
#test
{
    let x = 1

    #[Swag.Complete]
    switch x
    {
    case 0:
        @print("0")
    }
}
