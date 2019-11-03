#module compiler128

attr toto() -> "var"
attr toto1() -> "func"

#unittest error;
#[toto]
{
	#[toto1]
	func main () {}
}
