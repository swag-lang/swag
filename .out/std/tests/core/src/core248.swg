using Core, Hash

#test
{
    {
        digest := Sha256.compute("")
        const r = @[0xe3b0c442, 0x98fc1c14, 0x9afbf4c8, 0x996fb924, 0x27ae41e4, 0x649b934c, 0xa495991b, 0x7852b855]
        visit c, i: digest
            @assert(c == r[i])
    }

    {
        digest := Sha256.compute("t")
        const r = @[0xE3B98A4D, 0xA31A127D, 0x4BDE6E43, 0x033F66BA, 0x274CAB0E, 0xB7EB1C70, 0xEC41402B, 0xF6273DD8]
        visit c, i: digest
            @assert(c == r[i])
    }

    {
        digest := Sha256.compute("toto la foule")
        const r = @[0x4BA03FC6, 0x097E1248, 0x0C33D8C8, 0x8962F167, 0xD5CBF6D9, 0xA6CC0023, 0xBCD76B89, 0xE89B48F5]
        visit c, i: digest
            @assert(c == r[i])
    }
}