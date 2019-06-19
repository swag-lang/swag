#module module125;
#unittest pass semantic;

attr toto: () -> var;

#unittest error;
#[toto]
func main: ()
{
}

attr toto1: () -> func;

#unittest error;
#[toto1]
var a: bool;