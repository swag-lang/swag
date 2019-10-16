#module compiler80
;

namespace A
{
	typealias a: u32;
}

#unittest error;
namespace B
{
	var c: a;
}