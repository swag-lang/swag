#module module731

struct Array
{
	var ptr: *Array
	var value: s32 = 666
}

impl Array
{
	#[swag.printbc]
	func toto(self)
	{
		//self.ptr = &self
	}
}
	
#test
{
	var arr: Array
	arr.toto()
	//@assert(arr.ptr.value == 666)
}
