using swag
namespace A
{
    #[attributeUsage(AttributeUsage.Function)]
	attr toto();
	namespace B
	{
        #[attributeUsage(AttributeUsage.GlobalVariable)]
		attr toto1();
	}
}

#[A.toto]
func main () {}

#[A.B.toto1]
var a: bool;