#global testerror "Err0067"

#test
{
    let i = 0
    if i < 0:;
        // Missing `return false;`
}

/*
#test
{
    let i = 0
    if i < 0:
        // Missing `return false;`
}*/
