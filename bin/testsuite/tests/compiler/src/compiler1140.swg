//#global testerror "Err0720"

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