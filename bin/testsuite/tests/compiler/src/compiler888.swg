func arithmeticMean(numbers: const [..] f64) -> f64 {
    var total : f64 = 0
    loop numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

#test
{
	var result = arithmeticMean(@[1.0'f64, 2.0'f64, 3.0'f64, 4.0'f64, 5.0'f64])
	@assert(result == 3)
}