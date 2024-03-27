
using Core

func entry(th: Threading.Thread)
{
}

//#[Swag.PrintBc]
#test
{
    var th: Threading.Thread
    catch th.init(&entry)
    catch th.start()
    th.wait()
}
