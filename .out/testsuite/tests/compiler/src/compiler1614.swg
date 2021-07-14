//#[Swag.PrintBc]
#test
{
    var array: [10] u16

    var ptr  = &array[0]
    var ptr1 = &array[1]

    var diff = ptr1 - ptr
    @assert(diff == 1)
    @assert(ptr + diff == ptr1)
}