func toto()->const *u8
{
    var b = false
    var titi: const *u8
    var x = b ? null : titi
    return x
}

#test
{
    discard toto()
}