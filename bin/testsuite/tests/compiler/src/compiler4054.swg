#global testerror "Err0667"
struct(T) Arr {}

impl Arr
{
    mtd opDrop()
    where false
    {
        return 0
    }
}

struct N
{
    arr: Arr'N
}
