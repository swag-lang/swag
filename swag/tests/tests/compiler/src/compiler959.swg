#[swag.attributeUsage(swag.AttributeUsage.StructVariable)]
attr PR(x: s32);

struct toto
{
	#[PR(666)]
	x: s32
}

//#[swag.printbc]
#test
{
}