#global testerror "Err0055"

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
