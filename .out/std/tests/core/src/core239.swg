
using core

var G = 0
#test
{
    func entry(data: *void)
    {
        G = 666
    }

    var job: jobs.Job
    job.lambda = &entry
    jobs.scheduleJob(&job)
    jobs.waitDone()
    @assert(G == 666)
}