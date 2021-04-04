
#global testerror

//#[swag.printbc]
#[swag.optim("", false)]
func arithmeticMean(numbers: s32...)
{
    #[swag.safety("", true)]
    {
	    cpt := numbers[1]
    }
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}