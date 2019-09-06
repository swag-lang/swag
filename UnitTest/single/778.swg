#module module778

#[swag.foreign]
func GetStdHandle(handle: u32)->u32;

const STD_INPUT_HANDLE: u32 = cast(u32) -10

#[swag.printbc]
#test
{	
	h := GetStdHandle(STD_INPUT_HANDLE)
	@print(cast(s32) h)
}