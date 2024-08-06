#global testerror "Err0539"
#global testerror "Err0543"

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