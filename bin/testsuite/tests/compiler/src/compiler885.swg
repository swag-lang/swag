

//#[Swag.PrintBc]
#test
{
	currentValue := 3
	while currentValue
	{
		currentValue -= 1
	}

	@assert(currentValue == 0)
}
