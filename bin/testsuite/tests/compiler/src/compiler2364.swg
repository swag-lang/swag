interface ErrorId
{
    func toString(self)->string
}

struct ErrorString
{
    err: string
}

impl ErrorId for ErrorString
{
    func impl toString(using self)->string
    {
        return err
    }
}
/*
func toto()->ErrorId
{

}
*/

#test
{
}