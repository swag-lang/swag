#global testerror
struct(T) Arr {}

impl Arr
{
    mtd opDrop()
    #selectif false
    {
        return 0
    }
}

struct N
{
    arr: Arr'N
}
