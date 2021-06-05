#[Swag.attributeUsage(Swag.AttributeUsage.StructVariable)]
attr PR(x: s32);

struct toto
{
	#[PR(666)]
	x: s32
}

//#[Swag.printbc]
#test
{
}