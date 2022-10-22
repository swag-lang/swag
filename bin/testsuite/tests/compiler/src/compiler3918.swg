func toto(x: const *void) {}
func titi() {}

#test
{
    toto(acast &titi)
}