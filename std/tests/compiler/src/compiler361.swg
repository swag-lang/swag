#module "compiler361"
;

#unittest error;
#test
{
	const a = 1;
	var ptr = &a;
}
