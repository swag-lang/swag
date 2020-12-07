#module "compiler80"
;

namespace A
{
	alias a = u32;
}

#unittest error;
namespace B
{
	var c: a;
}