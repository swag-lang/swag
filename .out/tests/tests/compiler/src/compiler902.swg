
#global testerror

//#[Swag.printbc]
#[Swag.optim("", false)]
func arithmeticMean(numbers: s32...)
{
    #[Swag.safety("", true)]
    {
	    cpt := numbers[1]
    }
}

//#[Swag.printbc]
#test
{
	arithmeticMean(10)
}