#unittest module module128
#unittest pass semantic

attr toto: () -> var;
attr toto1: () -> func;

#unittest error
#[toto]
{
	#[toto1]
	func main: () {}
}
