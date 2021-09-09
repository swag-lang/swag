func toto()->const *u8
{
    b := false
    var titi: const *u8
    x := b ? titi : null
    return x
}

#test
{
    discard toto()
}