#global testerror "Err0402"
#test
{
    let a  = 0
    let b: const #decltype(a)
}
