#module module889

func arithmeticMean(numbers: [..] f64) -> f64 {
    var total : f64 = 0
    loop numbers
        total += numbers[index]
    return total / @countof(numbers)
}

#test
{
	let result = arithmeticMean([1, 2, 3, 4, 5])
	@assert(result == 3)
}