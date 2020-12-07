#module "compiler1047"

func new() => @{1}

#unittest error
//#[swag.printbc]
#test
{
	new().item0 = 1
}