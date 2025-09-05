using Core

// Helper function to convert digest to hex string
func digestToHex(digest: Hash.Sha1.Digest) -> String
{
    var result: String
    for i in 5
    {
        result += Format.toString("%{X08}", digest[i])
    }

    result.makeLower()
    return result
}

#test
{
    // Test 1: Empty string
    // Expected: da39a3ee5e6b4b0d3255bfef95601890afd80709
    {
        let input = ""
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        @assert(hex == "da39a3ee5e6b4b0d3255bfef95601890afd80709")
    }

    // Test 2: Single character "a"
    // Expected: 86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
    {
        let input = "a"
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        @assert(hex == "86f7e437faa5a7fce15d1ddcb9eaeaea377667b8")
    }

    // Test 3: "abc"
    // Expected: a9993e364706816aba3e25717850c26c9cd0d89d
    {
        let input = "abc"
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        @assert(hex == "a9993e364706816aba3e25717850c26c9cd0d89d")
    }

    // Test 4: Longer message
    // "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
    // Expected: 84983e441c3bd26ebaae4aa1f95129e5e54670f1
    {
        let input = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        @assert(hex == "84983e441c3bd26ebaae4aa1f95129e5e54670f1")
    }

    // Test 5: "The quick brown fox jumps over the lazy dog"
    // Expected: 2fd4e1c67a2d28fced849ee1bb76e7391b93eb12
    {
        let input = "The quick brown fox jumps over the lazy dog"
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        @assert(hex == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12")
    }

    // Test 6: Test incremental hashing (using init/update/final)
    {
        var sha1: Hash.Sha1
        sha1.init()
        sha1.update("The quick brown ")
        sha1.update("fox jumps over ")
        sha1.update("the lazy dog")

        var digest: Hash.Sha1.Digest
        sha1.final(digest)
        let hex = digestToHex(digest)
        @assert(hex == "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12")
    }

    // Test 7: Boundary test - exactly 55 bytes (triggers padding logic)
    {
        let input = "1234567890123456789012345678901234567890123456789012345"
        Debug.assert(@countof(input) == 55)
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
        // Expected result for this specific 55-byte string
    }

    // Test 8: Boundary test - exactly 64 bytes (one full block)
    {
        let input = "1234567890123456789012345678901234567890123456789012345678901234"
        Debug.assert(@countof(input) == 64)
        let digest = Hash.Sha1.compute(cast(const [..] u8) input)
        let hex = digestToHex(digest)
    }
}