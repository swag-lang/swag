/**
# `@drop` Intrinsic
The `@drop` intrinsic calls the `opDrop` method if it is defined for the struct. This ensures that any necessary cleanup operations (such as freeing resources) are performed before the variable is reinitialized. `@drop` is particularly useful in resource management, where explicit cleanup is required before resetting the variable.
*/

#test
{
    struct RGB { r = 1, g = 2, b = 3 }

    var rgb: [4] RGB

    // Calling `@drop` on the array. If `RGB` had an `opDrop` defined, it would be called here.
    @drop(&rgb, 4)

    @init(&rgb, 4)(5, 6, 7) // Reinitialize after dropping
    @assert(rgb[3].r == 5)
    @assert(rgb[3].g == 6)
    @assert(rgb[3].b == 7)
}
