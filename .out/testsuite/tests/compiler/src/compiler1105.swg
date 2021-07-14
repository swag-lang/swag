using Swag
#[AttrUsage(AttributeUsage.StructVariable)]
attr NC();

struct Toto
{
	#[NC]
	x: s32 = 10
}