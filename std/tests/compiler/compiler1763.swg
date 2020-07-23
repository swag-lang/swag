#[swag.printbc]
#test
{
    var array: [10] u16

    let ptr  = &array[0]
    let ptr1 = &array[1]

    let diff = ptr1 - ptr
    @print(cast(s64) diff)
    //@assert(ptr + diff == ptr1)
}