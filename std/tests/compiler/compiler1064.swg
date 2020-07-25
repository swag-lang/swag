//#[swag.constexpr]
//#[swag.printbc]
func fibonacci(num: s32) -> s32
{
    if num < 2
        return num
	else
        return fibonacci(num - 1) + fibonacci(num - 2)
}

var fibonacciOfAge: s32 = #run fibonacci(16)

#test
{
	@assert(fibonacciOfAge == 987)
}