#global testerror "Err0296"
internal const StringSmallSize = 16'u64

public struct String4725
{
    allocator:  Swag.IAllocator
    padding:    [StringSmallSize] u8
}