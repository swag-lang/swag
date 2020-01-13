using swag
#[attributeUsage(AttributeUsage.Field)]
attr NC();

struct Toto
{
	#[NC]
	x: s32 = 10
}