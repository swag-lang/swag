

attr PR(x: s32) -> "var"

struct toto
{
	#[PR(666)]
	x: s32
}

//#[swag.printbc]
#test
{
}