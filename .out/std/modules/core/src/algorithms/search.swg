#global export
#global namespace search

// Find 'value' in a slice by performing a binary search.
// The slice must be sorted in ascending order
func(T) sorted(values: const [..] T, value: T) -> {index: uint, success: bool}
{
    var result: retval

    min := 0'uint
    countof := @countof(values)
    max := @countof(values)

    while max >= min
    {
        mid := (max + min) / 2
        if mid >= countof
            break
        res := values[mid] <=> value
        if res == 0
        {
            result.index = mid
            result.success = true
            return result
        }

        if res < 0
            min = mid + 1
        else
            max = mid - 1
    }

    return result
}

// Find 'value' in a slice by performing a linear search.
// O^n, but slice does not have to be sorted
func(T) linear(values: const [..] T, value: T) -> {index: uint, success: bool}
{
    var result: retval
    visit v, i: values
    {
        if v == value
        {
            result.index = i
            result.success = true
            return result
        }
    }

    return result
}
