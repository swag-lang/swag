

func stepForward(input: s32) -> s32 {
    return input + 1
}
func stepBackward(input: s32) -> s32 {
    return input - 1
}

//#[Swag.PrintBc]
func chooseStepFunction(backward: bool) -> func(s32)->s32 {
	return backward ? &stepBackward : &stepForward
}

//#[Swag.PrintBc]
#test
{
	var currentValue = 3
	var moveNearerToZero = chooseStepFunction(backward: currentValue > 0)

	cpt := 0
	while currentValue != 0 {
		cpt += currentValue
		currentValue = moveNearerToZero(currentValue)
	}

	@assert(cpt == 6)
}
