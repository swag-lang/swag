func toto()->nl const *u8
{
    let b = false
    var titi: const *u8
    let x = b ? null : titi
    return x
}

#test
{
    discard toto()
}