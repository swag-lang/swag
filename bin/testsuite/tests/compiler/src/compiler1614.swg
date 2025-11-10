//#[Swag.PrintBc]
#test
{
    var array: [10] u16

    let ptr  = &array[0]
    let ptr1 = &array[1]

    let diff = ptr1 - ptr
    @assert(diff == 1)
    @assert(ptr + diff == ptr1)
}
