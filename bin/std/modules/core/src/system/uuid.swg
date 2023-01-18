#[Swag.Tls]
var g_RngUuid: Random.Mt64

public struct UUID
{
    using val: union
    {
        val8:  [16] u8
        val64: [2]  u64
    }
}

public impl UUID
{
    // Get a version 4 UUID (random)
    func v4()->UUID
    {
        var result: retval = ?
        result.val64[0] = g_RngUuid.nextU64()
        result.val64[1] = g_RngUuid.nextU64()
        result.val8[6] = (result.val8[6] & 0x0F) | 0x40 // Version 4
        result.val8[8] = (result.val8[8] & 0x3F) | 0x80 // Variant is 10
        return result
    }

    // Convert uuid to a string
    /*mtd toString()->String
    {
        var result: retval
        return result
    }*/
}