#module module897

func arithmeticMean(numbers: f64...) -> f64 {
    var total : f64 = 0
    loop numbers {
        total += numbers[index]
    }
    return total / @countof(numbers)
}

#test
{
	let result = arithmeticMean(1, 2)
	@assert(result == 1.5)
}