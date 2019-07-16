#module module432
//#unittest pass semantic

//#[swag.printbytecode]
#unittest error
#test
{
	var c: const [..] u8 = {0, 1}
	var d = cast(const [..] u16) c
}
