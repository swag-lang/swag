#module module134;
#unittest pass semantic;

//#unittest error;
namespace A
{
	namespace B
	{
		attr toto() -> var;
	}
	attr toto() -> func;
}
