

func arithmeticMean(numbers: f64...) -> f64 {
    var total: f64 = 0
    loop numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

#test
{
	res := arithmeticMean(1, 2, 3, 4, 5)
	@assert(res == 3)
	res1 := arithmeticMean(3, 8.25, 18.75)
	@assert(res1 == 10)
}