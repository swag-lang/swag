

struct Utf8
{
	size: u32
}

#unittest error
impl Utf8
{
	func reserveSize(self)
	{
		Utf8.size = 0
	}
}

#test
{
    var x: Utf8
    x.reserveSize()
}