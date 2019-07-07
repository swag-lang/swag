#module module307;
#unittest pass semantic;

func toto ()->bool { return true; }
func toto1 ()->bool { return true; }

#test
{
	@assert(toto == toto1);
}
