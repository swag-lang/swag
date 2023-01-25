#global testerror
struct(T) Arr {}

impl Arr
{
    mtd opDrop()
    #selectifonce false
    {
        return 0
    }
}

struct N
{
    arr: Arr'N
}
