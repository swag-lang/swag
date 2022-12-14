#global skip
struct(T) Arr {}

struct N
{
    arr: Arr'N
}

impl Arr
{
    mtd opPostCopy()
    {
        x := @typeof(T).flags
    }
}

#test
{
    var n: N
}