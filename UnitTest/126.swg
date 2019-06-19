#module module126;
#unittest pass semantic;

#unittest error;
attr toto: () -> func;
attr toto1: () -> var;

#[toto, toto1]
func main: ()
{
}
