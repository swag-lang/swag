func arithmeticMean(numbers: const [..] f64) -> f64 {
    var total : f64 = 0
    loop numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

#test
{
	var x = 1.0'f64
	var y = 2.0'f64
	var result = arithmeticMean(@[x, y])
	@assert(result == 1.5)
}