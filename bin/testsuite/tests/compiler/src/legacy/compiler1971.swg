struct(T) Array {}

var G = 0'u64

impl Array
{
    func validateCount(me)
    {
        //@print(#typeof(T).name)
        let size = cast(u64) #sizeof(T)
        G += size
    }

    func add(me)
    {
        //@print(#typeof(T).name)
        let size = #sizeof(T)
        G += cast(u64) size
        validateCount(me)
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
