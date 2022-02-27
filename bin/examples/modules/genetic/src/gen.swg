using Core

const PopulationCount = 200'u32
const TakeBest        = 40
const TakeWorst       = 5
const MaxShuffle      = 8'u32
const MutationRate    = 1

#[Swag.Tls]
var g_Rng:          Random.DefaultRng
var g_NumGenes:     u32
var g_Gen:          [2] Generation
var g_CurGen:       u32
var g_TotalWeight   = 0'f32
var g_BestScore     = Swag.F32.Max
var g_NumGen:       u32
var g_Result:       Array'u8

struct Generation
{
    all: ArrayPtr'Solution
}

struct Solution
{
    genes:  Array'u32
    score:  f32
    weight: f32
}

func genInit()
{
    assume Jobs.setNumWorkers()

    g_NumGenes = g_Image0.width * g_Image0.height

    // Generate initial population
    loop PopulationCount
    {
        soluce := g_Gen[0].all.addNewPtr()
        soluce.genes.resize(cast(uint) g_NumGenes)
        loop i: g_NumGenes
        {
            soluce.genes[i] = i
        }

        loop i: g_NumGenes
        {
            src := g_Rng.nextU32(0, cast(u32) g_NumGenes)
            dst := g_Rng.nextU32(0, cast(u32) g_NumGenes)
            swap(soluce.genes.buffer + src, soluce.genes.buffer + dst)
        }

        evaluateSolution(soluce)
    }

    // Sort solutions from best to worst
    g_Gen[0].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Placeholder for the next generation
    loop PopulationCount
    {
        soluce := g_Gen[1].all.addNewPtr()
        soluce.genes.resize(cast(uint) g_NumGenes)
    }

    g_Result.resize(cast(uint) g_NumGenes * 3)
}

func evaluateSolution(soluce: *Solution)
{
    soluce.score = 0
    loop i: soluce.genes
    {
        var a, b: Pixel.Color = ?

        idx := soluce.genes[i]

        a.r = g_Image0.pixels[(idx*3)+0]
        a.g = g_Image0.pixels[(idx*3)+1]
        a.b = g_Image0.pixels[(idx*3)+2]

        b.r = g_Image1.pixels[(i*3)+0]
        b.g = g_Image1.pixels[(i*3)+1]
        b.b = g_Image1.pixels[(i*3)+2]

        dist := a.getDistanceRgb(b)
        soluce.score += dist
    }

    soluce.score /= g_NumGenes
}

func pickToMate(other: *Solution = null)->*Solution
{
    rnd := g_Rng.nextF32(0, g_TotalWeight)
    visit child: g_Gen[g_CurGen].all
    {
        if rnd <= child.weight and child != other
            return child
        rnd -= child.weight
    }

    return g_Gen[g_CurGen].all[0]
}

func mutate(child: *Solution)
{
    rnd := g_Rng.nextU32(0, 100)
    if rnd <= MutationRate
    {
        rnd1 := g_Rng.nextU32(0, MaxShuffle)
        loop rnd1
        {
            src := g_Rng.nextU32(0, cast(u32) g_NumGenes)
            dst := g_Rng.nextU32(0, cast(u32) g_NumGenes)
            swap(child.genes.buffer + src, child.genes.buffer + dst)
        }
    }
}

func copyToNext(idxNext, idxCur: u32)
{
    nextGenIdx := (g_CurGen + 1) % 2
    g_Gen[nextGenIdx].all[idxNext].genes = g_Gen[g_CurGen].all[idxCur].genes
    evaluateSolution(g_Gen[nextGenIdx].all[idxNext])
}

func genGo()->*u8
{
    g_NumGen += 1

    // Compute stuff for selection
    g_TotalWeight = 0'f32
    visit child: g_Gen[g_CurGen].all
    {
        child.weight = 1 - child.score
        g_TotalWeight += child.weight
    }

    nextGenIdx := (g_CurGen + 1) % 2
    numChilds := PopulationCount - TakeBest - TakeWorst

    Jobs.parallelFor(|index| numChilds)
    {
        nextGenIdx := (g_CurGen + 1) % 2

        // Selection
        father := pickToMate()
        mother := pickToMate(father)

        // Cross point
        crossPoint1 := cast(uint) g_Rng.nextU32(1, cast(u32) g_NumGenes - 2)

        // Copy solution from father
        child1 := g_Gen[nextGenIdx].all[index]
        Memory.copy(child1.genes.buffer, father.genes.buffer, crossPoint1 * @sizeof(u32))

        var done: Array'bool
        done.resize(cast(uint) g_NumGenes)

        // Register father solution
        loop i: crossPoint1
            done[child1.genes.buffer[i]] = true

        // Copy solution from mother, in order, except of already present
        var holes: Array'u32
        cpt := 0
        loop i: g_NumGenes - crossPoint1
        {
            g := mother.genes[i + crossPoint1]
            if !done[g]
            {
                done[g] = true
                child1.genes[crossPoint1 + cpt] = g
            }
            else
            {
                holes.add(cast(u32) (crossPoint1 + cpt))
            }

            cpt += 1
        }

        // Fill the rest
        loop i: g_NumGenes
        {
            if done[i] continue
            idxHole := holes.popBack()
            child1.genes[idxHole] = i
            cpt += 1
        }

        mutate(child1)
        evaluateSolution(child1)
    }

    // Get from previous generation
    loop i: TakeBest
    {
        copyToNext(numChilds, cast(u32) i)
        numChilds += 1
    }

    loop i: TakeWorst
    {
        copyToNext(numChilds, PopulationCount - i - 1)
        numChilds += 1
    }

    g_CurGen = (g_CurGen + 1) % 2

    // Sort solutions from best to worst
    g_Gen[g_CurGen].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Returns the best score
    best      := g_Gen[g_CurGen].all[0]
    bestScore := best.score
    if bestScore < g_BestScore
    {
        g_BestScore = bestScore
        Console.print(g_NumGen, ": ", bestScore, "\n")

        loop i: best.genes
        {
            idx := best.genes[i]
            g_Result[(i*3)+0] = g_Image0.pixels[(idx*3)+0]
            g_Result[(i*3)+1] = g_Image0.pixels[(idx*3)+1]
            g_Result[(i*3)+2] = g_Image0.pixels[(idx*3)+2]
        }

        return g_Result.buffer
    }

    return null
}