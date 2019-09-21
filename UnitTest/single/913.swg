#module module913

//#[swag.printbc]
func(T) sum(x, y: T) => x > y

//#[swag.printbc]
#test
{
	@assert(sum(1, 3) == false)
	@assert(sum(3, 1) == true)
}