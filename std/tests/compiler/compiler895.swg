func arithmeticMean(numbers: const [..] f64) -> f64 {
    var total : f64 = 0
    loop numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

//#[swag.printbc]
#test
{
	var result = arithmeticMean(@[1, 2, 3, 4, 5])
	@assert(result == 3)
}