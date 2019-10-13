

struct Utf8
{
}

impl Utf8
{
	func opAffect(using self, from: string)
	{
		@assert(@countof(from) != 0)
		@assert(@dataof(from) != null)
		@assert(from != null)
	}
}

//#[swag.printbc]
#test
{
	var str: Utf8 = "null"
}