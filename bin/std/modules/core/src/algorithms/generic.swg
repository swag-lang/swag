#global export

// Swap two values
#[Swag.Inline]
func(T) swap(x, y: *T)
{
    var tmp =,moveraw dref x
    dref x =,nodrop,moveraw dref y
    dref y =,nodrop,move tmp
}

// Be sure that 'a' is lower or equal than 'b'
#[Swag.Inline]
func(T) orderMinMax(x, y: *T)
{
    if dref x > dref y do
        swap(x, y)
}

// Be sure that 'a' is geater or equal than 'b'
#[Swag.Inline]
func(T) orderMaxMin(x, y: *T)
{
    if dref x < dref y do
        swap(x, y)
}