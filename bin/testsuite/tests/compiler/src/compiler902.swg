
#global testerror "Saf0004"

//#[Swag.PrintBc]
#[Swag.Optimize("", false)]
func arithmeticMean(numbers: s32...)
{
    #[Swag.Safety("", true)]
    {
	    var cpt = numbers[1]
    }
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(10)
}