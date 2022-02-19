// You can get an interface to the compiler with '@compiler', and use
// 'compileString' to request a build.
#run
{
    itf := @compiler()

    // Creates a global constant named 'XX'
    itf.compileString("const XX = 666")
}

// We need a #placeholder so that every piece of code knows that 'XX' will
// exist at some point, and must wait if necessary until it does.
#placeholder XX

#test
{
    @assert(XX == 666)
}