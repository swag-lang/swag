#module module1006
using swag

//#[swag.printbc]
#test
{
	let x: any = "6"
	@assert(cast(string) x == "6")
}
