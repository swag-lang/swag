using swag
#[attributeUsage(AttributeUsage.Struct)]
attr PR(x: s32);

#[PR(666)]
struct toto
{
	x: s32
}

//#[swag.printbc]
#test
{
}