
#global testerror

//#[swag.printbc]
#[swag.optim("", false)]
func arithmeticMean(numbers: ...)
{
    #[swag.safety("", true)]
	cpt := cast(s32) numbers[1]
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}