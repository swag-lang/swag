#module module249;
#unittest pass semantic;
using swag;

#[constexpr]
func value ()->s32 { return 55; }

func main (a: s32 = value())->s32
{ 
	return a;
}

//#[printbytecode]
func main1 ()
{
	@assert(main == 55);
}

#run main1;