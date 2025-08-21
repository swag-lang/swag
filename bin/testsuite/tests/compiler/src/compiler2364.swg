interface ErrorId
{
    func toString(me)->string;
}

struct ErrorString
{
    err: string
}

impl ErrorId for ErrorString
{
    func impl toString(using me)->string
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