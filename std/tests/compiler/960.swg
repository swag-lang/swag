#module module960

attr PR(x: s32) -> struct

#[PR(666)]
struct toto
{
	x: s32
}

//#[swag.printbc]
#test
{
}