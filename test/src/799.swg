#module module799

struct Utf8
{
	size: u32
}

#unittest error
impl Utf8
{
	func reserveSize(self)
	{
		size = 0
	}
}
