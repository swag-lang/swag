func arithmeticMean(numbers: const [..] f64)->f64
{
    var total: f64 = 0
    for numbers
    {
        total += numbers[@index]
    }
    return total / @countof(numbers)
}

#test
{
    let x      = 1.0'f64
    let y      = 2.0'f64
    let result = arithmeticMean([x, y])
    @assert(result == 1.5)
}
