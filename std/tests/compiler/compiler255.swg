#module compiler255
;
using swag;

#unittest error
namespace A
{
}

func main ()
{
	A.@assert(true);
}
