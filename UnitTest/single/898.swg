#module module898

func arithmeticMean(numbers: f32...)
{
	@assert(@countof(numbers) == 2)
}

#test
{
	arithmeticMean(1, 2)
}