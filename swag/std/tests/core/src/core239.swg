#import "core"
using core

#test
{
    func entry(data: *void)
    {
        @print("entry\n")
    }

    var v: swag.SystemAllocator
    system.pushAllocator(v)
    {
        try jobsystem.init(4)
        job := jobsystem.createJob(&entry)
        jobsystem.scheduleJob(job)
        thread.sleep(1000)
    }
}