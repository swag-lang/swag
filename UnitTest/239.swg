#unittest module module239;
#unittest pass semantic;
using swag;

func main1: ()->s32 { return 77; }

#[ConstExpr]
func main2: ()->s32 { return 666; }

//#[PrintByteCode]
func main: ()
{
	var a = #run main1();
	var b = main1();
	var c = main2();
	@assert(a == 77);
	@assert(b == 77);
	@assert(c == 666);
}

#run main;