struct(T) Array
{
}

var G = 0'uint

impl Array
{
	func validateCount(self)
	{
        //@print(@typeof(T).name)
	    size := cast(uint) @sizeof(T)
        G += size
    }

	func add(self)
	{
        //@print(@typeof(T).name)
        size := @sizeof(T)
        G += cast(uint) size
		validateCount(self)
	}
}

//#[Swag.PrintBc]
#test
{
	var arr: Array's32
	Array.add(&arr)

    var arr1: Array'bool
    Array.add(&arr1)

    @assert(G == 4 + 4 + 1 + 1)
}