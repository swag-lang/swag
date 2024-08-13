//#global testerror "Err0700"

namespace A.B
{
	using A.C
}

namespace A.B
{
	func toto1() => toto()
}

namespace A.C
{
	func toto()
	{
	}
}