#global testerror

struct Utf8
{
	size: u32
}

impl Utf8
{
	func reserveSize(self)
	{
		size = 0
	}
}

#test
{
    var x: Utf8
    x.reserveSize()
}