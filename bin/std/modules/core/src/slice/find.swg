#global export
#global namespace Slice

// Find 'value' in a slice by performing a binary search.
// The slice must be sorted in ascending order
// Returns Swag.U64.Max if not found.
func(T, V) findSorted(values: const [..] T, value: V)->u64
{
    var result: retval

    var min = 0'u64
    let countof = @countof(values)
    var max = @countof(values)

    while max >= min
    {
        let mid = (max + min) / 2
        if mid >= countof
            break
        let res = values[mid] <=> value
        if res == 0
            return mid

        if res < 0
            min = mid + 1
        else
            max = mid - 1
    }

    return Swag.U64.Max
}

// Find 'value' in a slice by performing a linear search.
// O^n, but slice does not have to be sorted.
// Returns Swag.U64.Max if not found.
func(T, V) findLinear(values: const [..] T, value: V)->u64
{
    visit v, i: values
    {
        if v == value
            return i
    }

    return Swag.U64.Max
}

// Returns true if the given slice contains the 'value'
#[Swag.ConstExpr]
func(T, V) contains(values: const [..] T, value: V)->bool
{
    visit v: values
    {
        if v == value
            return true
    }

    return false
}

// Returns true if two slices are equal
#[Swag.ConstExpr]
func(T) equals(dst, src: const [..] T)->bool
{
    if @countof(dst) != @countof(src)
        return false
    loop i: dst
    {
        if dst[i] != src[i]
            return false
    }

    return true
}
