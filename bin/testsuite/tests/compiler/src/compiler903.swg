
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func arithmeticMean(numbers: ...)
{
    #[Swag.Safety("", true)]
	cpt := cast(s32) numbers[1]
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(10)
}