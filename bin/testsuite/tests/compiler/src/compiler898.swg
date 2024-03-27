

func arithmeticMean(numbers: f32...)
{
	@assert(@countof(numbers) == 3)
}

//#[Swag.PrintBc]
#test
{
	arithmeticMean(1, 2, 3)
}