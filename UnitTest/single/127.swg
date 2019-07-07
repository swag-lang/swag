#module module127;
#unittest pass semantic;

attr toto: () -> func;

#[toto]
func main ()
{
}

attr toto1: () -> var;
#[toto1]
var a: bool;