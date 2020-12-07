#module "compiler214"
#unittest pass syntax;

#run
{
	var a = 1;
	if(true)
	{
		var a: bool = 1;
		@print("titi");
	}
}