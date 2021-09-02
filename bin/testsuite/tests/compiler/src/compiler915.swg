func arithmeticMean(numbers: const [..] f64) -> f64 {
    var total : f64 = 0
    loop numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

//#[Swag.PrintBc]
#test
{
	var result = arithmeticMean(@[1.0, 2.0, 3.0, 4, 5])
	@assert(result == 3)
}