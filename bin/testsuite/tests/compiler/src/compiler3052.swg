struct Variant {}

impl Variant
{
    #[Swag.ConstExpr]
    func checkValidType(a: s32, b = true) {}

    // Set the variant value
    mtd set()
    {
        checkValidType(0)
    }
}

#test
{
    var v: Variant
    v.set()
}
