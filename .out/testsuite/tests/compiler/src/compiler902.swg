
#global testerror

//#[Swag.PrintBc]
#[Swag.Optim("", false)]
func arithmeticMean(numbers: s32...)
{
    #[Swag.Safety("", true)]
    {
	    cpt := numbers[1]
    }
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(10)
}