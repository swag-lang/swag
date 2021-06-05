

//#[Swag.printbc]
func arithmeticMean(x: string, numbers: f32...)
{
	@assert(x == "toto")
	@assert(numbers[0] == 10.0)
	@assert(numbers[1] == 20.0)
}

//#[Swag.printbc]
#test
{
	arithmeticMean("toto", 10, 20)
}