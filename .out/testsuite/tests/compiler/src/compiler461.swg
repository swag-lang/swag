
#global testpass syntax

//#[Swag.PrintBc]
#test
{
	var a: func()->s32
	var b: func()
	var c: func(s32, s32)
	var c: func(s32, s32) -> func()->s32
	var c: func(func()->s32, s32) -> func()->s32
}