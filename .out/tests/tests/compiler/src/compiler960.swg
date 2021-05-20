using Swag
#[attributeUsage(AttributeUsage.Struct)]
attr PR(x: s32);

#[PR(666)]
struct toto
{
	x: s32
}

//#[Swag.printbc]
#test
{
}