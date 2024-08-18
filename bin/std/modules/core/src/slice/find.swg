#global export
#global namespace Slice

// Find 'value' in a slice by performing a binary search.
// The slice must be sorted in ascending order
// Returns Swag.U64.Max if not found.
func(T, V) findSorted(values: const [..] T, value: V)->u64
{
    var result: retval

    var min     = 0'u64
    let countof = @countof(values)
    var max     = @countof(values)

    while min <= max
    {
        let mid = (max + min) / 2
        if mid >= countof do
            break

        let res = values[mid] <=> value
        if res == 0 do
            return mid
        if res < 0 do
            min = mid + 1
        elif mid == 0 do
            break
        else do
            max = mid - 1
    }

    return Swag.U64.Max
}

// Find 'value' in a slice by performing a linear search.
// O^n, but slice does not have to be sorted.
// Returns Swag.U64.Max if not found.
func(T, V) findLinear(values: const [..] T, value: V)->u64
{
    visit v, i in values where v == value do
        return i
    return Swag.U64.Max
}

// Returns true if the given slice contains the sub slice 'value'
func(T) findSlice(values, value: const [..] T)->u32
{
    if @countof(value) > @countof(values) do
        return Swag.U32.Max

    loop it0 in 0 to @countof(values) - @countof(value)
    {
        var ok = true
        visit c, it1 in value
        {
            if values[it0 + it1] != c
            {
                ok = false
                break
            }
        }

        if ok do
            return acast it0
    }

    return Swag.U32.Max
}

// Returns true if the given slice contains the 'value'.
// The slice must be sorted in ascending order
#[Swag.ConstExpr]
func(T, V) containsSorted(values: const [..] T, value: V)->bool
{
    return findSorted(values, value) != Swag.U64.Max
}

// Returns true if the given slice contains the 'value'
#[Swag.ConstExpr, Swag.Overload]
func(T, V) contains(values: const [..] T, value: V)->bool
{
    visit v in values where v == value do
        return true
    return false
}

// Returns true if two slices are equal
#[Swag.ConstExpr]
func(T) equals(dst, src: const [..] T)->bool
{
    if @countof(dst) != @countof(src) do
        return false
    loop i in dst where dst[i] != src[i] do
        return false
    return true
}
