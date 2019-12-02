#module compiler125

attr toto() -> "var"

#unittest error;
#[toto]
func main ()
{
}

attr toto: () -> "func"

/*#unittest error;
#[toto1]
var a: bool;*/