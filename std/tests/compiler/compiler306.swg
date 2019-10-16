#module compiler306
#unittest pass semantic;

#unittest error;
#test
{
	@assert("toto" > "toto");
}
