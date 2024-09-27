func minMax(array: nn const [..] s32) -> {min: s32, max: s32}
{
    var currentMin, currentMax = array[0]
    for array
	{
		let value = array[#index]
        if value < currentMin:
            currentMin = value
        elif value > currentMax:
            currentMax = value
    }

    return {currentMin, currentMax}
}

//#[Swag.PrintBc]
#test
{
	var bounds = minMax(array: [8, -6, 2, 109, 3, 71])
	@assert(bounds.min == -6)
	@assert(bounds.max == 109)
}