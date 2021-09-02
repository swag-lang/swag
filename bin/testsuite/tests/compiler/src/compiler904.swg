

//#[Swag.PrintBc]
func arithmeticMean(x: string, numbers: f32...)
{
	@assert(x == "toto")
	@assert(numbers[0] == 10.0)
	@assert(numbers[1] == 20.0)
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean("toto", 10, 20)
}