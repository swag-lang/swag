#import "core"
using core

var G = 0
#test
{
    func entry(data: *void)
    {
        G = 666
    }

    var job: jobsystem.Job
    job.lambda = &entry
    jobsystem.scheduleJob(&job)
    jobsystem.waitDone()
    @assert(G == 666)
}