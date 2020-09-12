struct(T) Array
{
}

var G = 0'u32

impl Array
{
	func validateCount(self)
	{
        //@print(@typeof(T).name)
	    size := @sizeof(T)
        G += size
    }

	func add(self)
	{
        //@print(@typeof(T).name)
        size := @sizeof(T)
        G += size
		validateCount(self)
	}
}

//#[swag.printbc]
#test
{
	var arr: Array's32
	Array.add(&arr)

    var arr1: Array'bool
    Array.add(&arr1)

    @assert(G == 4 + 4 + 1 + 1)
}