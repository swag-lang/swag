struct(T) Arr
{
}

impl Arr
{
    #[Swag.Inline, Swag.Overload]
    mtd opIndex(x: u64)->T
    {
        return {0}
    }
}

#[Swag.Optim("", false)]
func toto()
{
    var arr0: Arr'{x: s32}
    var arr1: Arr'{y: s32}
    let a = arr0[0's32]
    let b = arr1[0's32]
}
