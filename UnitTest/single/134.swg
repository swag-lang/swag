#module module134;
#unittest pass semantic;

#unittest error;
namespace A
{
	attr toto() -> func;
	namespace B
	{
		attr toto() -> var;
	}
}
