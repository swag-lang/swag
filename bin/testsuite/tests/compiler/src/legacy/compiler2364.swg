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
    func impl toString(me)->string
    {
        return me.err
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
