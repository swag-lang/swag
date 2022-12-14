// :TOFIX
#global skip
struct(T) Arr {}

impl Arr
{
    mtd opPostCopy() #selectif @typeof(T).flags == 0
    {
    }
}

struct N
{
    arr: Arr'N
}
