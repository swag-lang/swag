var decimalDouble = 12.1875
var exponentDouble = 1.21875e1
var paddedDouble = 000123.456
var oneMillion = 1_000_000
var justOverOneMillion = 1_000_000.000_000_1
var orangesAreOrange = true
var turnipsAreDelicious = false

var http404Error = @{404, "Not Found"}
#test
{
    var (statusCode, statusMessage) = http404Error
    @assert(statusCode == 404)
    @assert(statusMessage == "Not Found")
}
