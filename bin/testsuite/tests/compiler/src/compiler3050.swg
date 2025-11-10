/*var G: s32

struct XX {}
impl XX
{
    mtd opDrop() { G += 1; }
    mtd opPostCopy() {G += 1; }
    mtd opPostMove() {G += 1; }
}

struct(T: const [..] typeinfo) Variant
{
}

impl Variant
{
    #[Swag.ConstExpr]
    func orFlags()->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = Zero
        foreach p in Variant.T:
            res |= p.flags
        return res
    }

    mtd opPostCopy()
    {
        #if orFlags() & .HasPostCopy
        {
            G += 1
        }
    }

    mtd opPostMove()
    {
        #if orFlags() & .HasPostMove
        {
            G += 1
        }
    }

    mtd opDrop()
    {
        #if orFlags() & .HasDrop
        {
            G += 1
        }
    }
}

#test
{
    var v: Variant'([XX])
    var v1: Variant'([string])
}*/
