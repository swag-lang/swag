#global testerror "Saf0008"

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func arithmeticMean(numbers: ...)
{
    #[Swag.Safety("", true)]
	var cpt = cast(s32) numbers[1]
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(10)
}