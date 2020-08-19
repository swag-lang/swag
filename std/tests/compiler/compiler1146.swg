#module "compiler1146"
#unittest error

#[swag.compiler]
func toto()
{
	//@print(x)
}

func toto1()
{
	x:= &toto
}
