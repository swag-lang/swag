//#module "compiler902"
//#unittest error

//#[swag.printbc]
func arithmeticMean(x: string, numbers: string...)->string
{
    @assert(x == "666")
    @assert(numbers[0] == "10")
    @assert(numbers[1] == "20")
    return "0"
}

//#[swag.printbc]
#test
{
	arithmeticMean("666", "10", "20")
}