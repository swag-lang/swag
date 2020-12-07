#module "compiler215"
;

#unittest error;
#run
{
	var a = 1;
	if(true)
	{
		var a: bool = true;
	}
}