#module module528
#unittest pass semantic

struct toto
{
}

impl toto
{
	func f()
	{
	}
}

#test
{
	toto.f()
}