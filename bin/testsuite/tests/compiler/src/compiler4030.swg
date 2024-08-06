#global testerror "Err0537"
#global testerror "Err0541"

struct Utf8
{
	size: u32
}

impl Utf8
{
	func reserveSize()
	{
		size = 0
	}
}

#test
{
    var x: Utf8
    x.reserveSize()
}