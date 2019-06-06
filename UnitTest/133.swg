#unittest module module133;
#unittest pass semantic;

namespace A
{
	attr toto: () -> func;
	namespace B
	{
		attr toto1: () -> var;
	}
}

#[A.toto]
func main: () {}

#[A.B.toto1]
var a: bool;