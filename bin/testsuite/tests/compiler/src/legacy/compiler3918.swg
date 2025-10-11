func toto(x: const *void) {}
func titi() {}

#test
{
    toto(cast() &titi)
}
