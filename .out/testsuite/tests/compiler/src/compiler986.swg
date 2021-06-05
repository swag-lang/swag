//#[Swag.printbc]
#test
{
	pair := @{"toto", {10, 20}}
	@assert(pair.item1.item0 == 10)
	@assert(pair.item1.item1 == 20)
}
