#global testerror "Err0659"
struct(T) Arr {}

impl Arr
{
    mtd opDrop()
    verify false
    {
        return 0
    }
}

struct N
{
    arr: Arr'N
}
