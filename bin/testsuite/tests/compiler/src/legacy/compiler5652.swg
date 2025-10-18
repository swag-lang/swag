#global testerror "Err0412"

#test
{
    let i = 0
    if i < 0 do;
        // Missing `return false;`
}

/*
#test
{
    let i = 0
    if i < 0:
        // Missing `return false;`
}*/
