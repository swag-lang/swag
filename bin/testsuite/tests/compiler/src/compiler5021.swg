#global testerror "Err0439"

#run
{
    let a = 0
    switch a
    {
    default:
        fallthrough
    }
}