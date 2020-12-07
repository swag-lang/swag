#[swag.attributeUsage(swag.AttributeUsage.Field)]
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