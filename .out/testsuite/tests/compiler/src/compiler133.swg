using Swag
namespace A
{
    #[AttrUsage(AttributeUsage.Function)]
	attr toto();
	namespace B
	{
        #[AttrUsage(AttributeUsage.GlobalVariable)]
		attr toto1();
	}
}

#[A.toto]
func main () {}

#[A.B.toto1]
var a: bool;