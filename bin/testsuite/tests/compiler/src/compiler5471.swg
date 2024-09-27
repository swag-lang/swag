// Swap two values
#[Swag.Inline]
func(T) swap(x, y: *T)
{
    var tmp = #moveraw dref x
    dref x = #nodrop #moveraw dref y
    dref y = #nodrop #move tmp
}

// Be sure that 'a' is lower or equal than 'b'
#[Swag.Inline]
func(T) orderMinMax(x, y: *T)
{
    if dref x > dref y:
        swap(x, y)
}

private struct Bot
{
    id:          s32
    count:       s32
    val:         [2] s32 = Swag.S32.Max
    out:         s32
}

private impl Bot
{
    mtd add(v: s32)
    {
        val[count] = v
        orderMinMax(&val[0], &val[1])
    }
}

