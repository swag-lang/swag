#module "compiler1145"
#unittest error

#[swag.compiler, swag.inline]
func toto()
{
	//@print(x)
}

func toto1()
{
	toto()
}
