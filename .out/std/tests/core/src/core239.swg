
using core

var G = 0
#test
{
    func entry(data: *void)
    {
        G = 666
    }

    var job: Jobs.Job
    job.lambda = &entry
    Jobs.scheduleJob(&job)
    Jobs.waitDone()
    @assert(G == 666)
}