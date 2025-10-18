#global testerror "Err0103"
//#global skip
/* salut la foule
    /* */
*/
func toto() throw
{
    let ext = ".wav"
    switch ext
    {
    case ".wav":
        break
    default:
        throw Swag.Error{}
    }
}
