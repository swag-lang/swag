#module "compiler134"

#unittest error;
namespace A
{
	namespace B
	{
		attr toto() -> "var"
	}
	attr toto() -> "func"
}
