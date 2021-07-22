var G: s32

struct XX {}
impl XX
{
    method opDrop() { G += 1; }
    method opPostCopy() {G += 1; }
    method opPostMove() {G += 1; }
}

struct(T: const [..] typeinfo) Variant
{
}

impl Variant
{
    #[Swag.ConstExpr]
    func orFlags()->Swag.TypeInfoFlags
    {
        var res: Swag.TypeInfoFlags = None
        visit p: T
            res |= p.flags
        return res
    }

    method opPostCopy()
    {
        #if orFlags() & .HasPostCopy
        {
            G += 1
        }
    }

    method opPostMove()
    {
        #if orFlags() & .HasPostMove
        {
            G += 1
        }
    }

    method opDrop()
    {
        #if orFlags() & .HasDrop
        {
            G += 1
        }
    }
}

#test
{
    var v: Variant'(@[XX])
    var v1: Variant'(@[string])
}