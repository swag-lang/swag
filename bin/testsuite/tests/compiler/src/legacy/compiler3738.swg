#global testerror "Saf0029"

//#[Swag.PrintBc, Swag.Optimize("", false)]
#[Swag.Safety(.All, .All, true)]
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
