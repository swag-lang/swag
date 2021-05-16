#global export

// Swap two values
#[swag.inline]
func(T) swap(x, y: *T)
{
    tmp :=,moveraw dref x
    dref x =,nodrop,moveraw dref y
    dref y =,nodrop,moveraw tmp
}
