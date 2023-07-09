func toto()->const *u8
{
    var b = false
    var titi: const *u8
    var x = b ? titi : null
    return x
}

#test
{
    discard toto()
}