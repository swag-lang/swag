using Swag
#[AttrUsage(AttributeUsage.Function)]
attr toto();

#[toto]
func main ()
{
}

#[AttrUsage(AttributeUsage.GlobalVariable)]
attr toto1();

#[toto1]
var a: bool;