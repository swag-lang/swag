#module module80;
#unittest pass semantic;

namespace A
{
	typealias a: u32;
}

#unittest error;
namespace B
{
	var c: a;
}