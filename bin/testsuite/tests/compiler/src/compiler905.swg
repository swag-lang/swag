

func arithmeticMean(numbers: f64...) -> f64 {
    var total: f64 = 0
    for numbers {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

//#[Swag.PrintBc]
#test
{
	let res = arithmeticMean(1, 2, 3, 4, 5)
	@assert(res == 3)
	let res1 = arithmeticMean(3, 8.25'f64, 18.75'f64)
	@assert(res1 == 10)
}