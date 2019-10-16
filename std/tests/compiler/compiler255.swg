#module compiler255
#unittest pass semantic;
using swag;

#unittest error
namespace A
{
}

func main ()
{
	A.@assert(true);
}
