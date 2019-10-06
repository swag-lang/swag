#module module1041

#[swag.printbc]
#test
{
	var pt : s32
	ptr := &pt
	drop(ptr)
}