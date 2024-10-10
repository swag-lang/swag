#global testerror "Err0070"

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
