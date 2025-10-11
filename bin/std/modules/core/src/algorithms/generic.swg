#global export

// Swaps the values of two variables.
// Takes pointers to the two values to be swapped.
#[Swag.Inline]
func(T) swap(x, y: *T)
{
    var tmp = #moveraw dref x
    dref x = #nodrop #moveraw dref y
    dref y = #nodrop #move tmp
}

// Ensures that the first value is less than or equal to the second value.
// If the first value is greater than the second, their contents are swapped.
#[Swag.Inline]
func(T) orderMinMax(x, y: *T)
{
    if dref x > dref y do
        swap(x, y)
}

// Ensures that the first value is greater than or equal to the second value.
// If the first value is less than the second, their contents are swapped.
#[Swag.Inline]
func(T) orderMaxMin(x, y: *T)
{
    if dref x < dref y do
        swap(x, y)
}
