#module "compiler1102"

//#[swag.enumflags]
enum Days
{
	A = 1
}

#unittest error
#test
{
	x := ~Days.A
}