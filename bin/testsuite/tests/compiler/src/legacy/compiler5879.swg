#global testerror "Err0699"

struct Utf8
{
	size: u32
}

impl Utf8
{
	func reserveSize()
	{
		me.size = 0
	}
}
/*
#test
{
    var x: Utf8
    x.reserveSize()
}*/