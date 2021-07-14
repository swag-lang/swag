
#global testerror

//#[Swag.PrintBc]
#[Swag.optim("", false)]
func arithmeticMean(numbers: s32...)
{
    #[Swag.safety("", true)]
    {
	    cpt := numbers[1]
    }
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(10)
}