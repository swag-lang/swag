//#global skip
struct(T) Arr {}

struct N
{
    arr: Arr'N
}

impl Arr
{
    mtd opPostCopy()
    {
        var x = #typeof(T).flags
    }
}

#test
{
    var n: N
}
