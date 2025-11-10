struct Array {}

impl Array
{
    #[Swag.Macro]
    mtd(ptr: bool, back: bool) opVisit(stmt: #code void)
    {
        for 7 do
            #inject(stmt)
    }

    mtd opSlice(a, b: u64)->string
    {
        return "ABC"
    }

    mtd opCount() => 3'u64
}

#[Swag.Optimize(.All, false)]
#test
{
    var arr: Array
    var g    = 0
    foreach arr
    {
        foreach arr[1 to ]
        {
            g += 1
            break
        }
    }

    @assert(g == 7)
}
