#module "compiler285"
#unittest pass syntax;

#run
{
	a = a[4];
	a = a[4 + 1];
	a = &a[0];
}