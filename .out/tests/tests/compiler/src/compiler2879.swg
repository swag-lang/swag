#global testerror

#test
{
    visit a, b, c: "toto"
    {

    }
}

// erreur si a l'appel d'une macro on utilise des alias qui ne sont pas utilisés dans la macro
// erreurs si on declare un @alias1 sans utiliser @alias0 dans une macro