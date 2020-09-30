func minMax(array: const [..] s32) -> struct{min: s32, max: s32}
{
    var currentMin, currentMax = array[0]
    loop array
	{
		var value = array[@index]
        if value < currentMin
            currentMin = value
        else if value > currentMax
            currentMax = value
    }

    return @{currentMin, currentMax}
}

//#[swag.printbc]
#test
{
	var bounds = minMax(array: @[8, -6, 2, 109, 3, 71])
	@assert(bounds.min == -6)
	@assert(bounds.max == 109)
}