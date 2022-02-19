// Order of top level declarations does not matter
const A = B
const B = C
const C = 1 // C could also be defined in another file.

#test
{
    funcDeclaredLater()
}

func funcDeclaredLater() {}