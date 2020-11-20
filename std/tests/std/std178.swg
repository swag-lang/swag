#import "std"
using std

func entry(th: thread.Thread)
{
    //loop 10 @print("toto\n")
}

//#[swag.printbc]
#test
{
    th := thread.create(&entry)
    th.start()
    //while !th.isDone()
    //    @print("titi\n")
    th.wait()
}
