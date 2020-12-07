#module "compiler93"
;

namespace A
{
	namespace B
	{
		var tt1: bool;
	}
}

#unittest error;
var a: A.B.tt1 = true;
#unittest error;
var b: A;