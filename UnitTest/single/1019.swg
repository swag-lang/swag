#module module1019
using swag

#[swag.printbc]
#test
{
	ptr := @getcontext()
	@print(cast(s32) ptr.toto)
}