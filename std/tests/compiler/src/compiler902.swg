
#unittest error

//#[swag.printbc]
func arithmeticMean(numbers: s32...)
{
    #[swag.safety(true)]
    {
	    cpt := numbers[1]
    }
}

//#[swag.printbc]
#test
{
	arithmeticMean(10)
}