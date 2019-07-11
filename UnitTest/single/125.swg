#module module125;
#unittest pass semantic;

attr toto() -> var;

#unittest error;
#[toto]
func main ()
{
}

attr toto: () -> func;

#unittest error;
#[toto1]
var a: bool;