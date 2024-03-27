
using Core

var th1, th2: Threading.Thread

func entry1(th: Threading.Thread)
{
    //@print("entry1\n")
}

func entry2(th: Threading.Thread)
{
    //@print("entry2\n")
    th1.wait()
    //@print("end entry2\n")
}

//#[Swag.PrintBc]
#test
{
    catch th1.init(&entry1)
    catch Threading.Thread.init(&th2, &entry2)
    catch th1.start()
    catch th2.start()
    th2.wait()
    //@print("end\n")
}
