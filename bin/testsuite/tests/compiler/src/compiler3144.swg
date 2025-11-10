func toto()->#null const *u8
{
    let b     = false
    var titi: const *u8
    let x     = b ? titi : null
    return x
}

#test
{
    discard toto()
}
