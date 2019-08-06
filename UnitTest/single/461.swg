#module module461
#unittest pass syntax

//#[swag.printbc]
#test
{
	var a: ()->s32;
	var b: ();
	var c: (s32, s32);
	var c: (s32, s32) -> ()->s32;
	var c: (()->s32, s32) -> ()->s32;
}