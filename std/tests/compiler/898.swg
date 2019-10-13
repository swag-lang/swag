

func arithmeticMean(numbers: f32...)
{
	@assert(@countof(numbers) == 3)
}

//#[swag.printbc]
#test
{
	arithmeticMean(1, 2, 3)
}