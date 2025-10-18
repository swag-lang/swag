#global testerror "Err0121"
internal const StringSmallSize = 16'u64

public struct String4725
{
    allocator:     Swag.IAllocator
    padding:       [StringSmallSize] u8
}
