let decimalDouble = 12.1875
let exponentDouble = 1.21875e1
let paddedDouble = 000123.456
let oneMillion = 1_000_000
let justOverOneMillion = 1_000_000.000_000_1
let orangesAreOrange = true
let turnipsAreDelicious = false

let http404Error = {404, "Not Found"}
#test
{
    let (statusCode, statusMessage) = http404Error
    @assert(statusCode == 404)
    @assert(statusMessage == "Not Found")
}
