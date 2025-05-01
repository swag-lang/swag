struct Array {}

impl Array
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #block void)
    {
        for 7:
        #inject stmt
    }

    mtd opSlice(a, b: u64)->string
    {
        return "ABC"
    }

    mtd opCount() => 3'u64
}

#[Swag.Optimize("", false)]
#test
{
    var arr: Array
    var g = 0
    foreach arr
    {
        foreach arr[1..]
        {
            g += 1
            break
        }
    }

    @assert(g==7)
}