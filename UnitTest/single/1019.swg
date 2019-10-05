#module module1019
using swag

func me(x: string)
{
	@print("titi")
}

//#[swag.printbc]
#test
{
	ptr := @getcontext()
	ptr.allocator("toto")
}