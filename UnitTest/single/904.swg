#module module904

//#[swag.printbc]
func arithmeticMean(x: string, numbers: f32...)
{
	@assert(x == "toto")
	@assert(numbers[0] == 10.0)
}

//#[swag.printbc]
#test
{
	arithmeticMean("toto", 10)
}