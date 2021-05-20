
#global testerror

//#[Swag.printbc]
#[Swag.optim("", false)]
func arithmeticMean(numbers: ...)
{
    #[Swag.safety("", true)]
	cpt := cast(s32) numbers[1]
}

//#[Swag.printbc]
#test
{
	arithmeticMean(10)
}