#module "compiler461"
#unittest pass syntax

//#[swag.printbc]
#test
{
	var a: func()->s32
	var b: func()
	var c: func(s32, s32)
	var c: func(s32, s32) -> func()->s32
	var c: func(func()->s32, s32) -> func()->s32
}