#global testerror "Err0661"
struct(T) Arr {}

impl Arr
{
    mtd opDrop()
    #validif false
    {
        return 0
    }
}

struct N
{
    arr: Arr'N
}
