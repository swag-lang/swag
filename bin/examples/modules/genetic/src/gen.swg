using Core

struct Generation
{
    all: Array'(*Solution)
}

const PopulationCount = 400'u32
const TakeBest        = 40
const TakeWorst       = 4
const MaxShuffle      = 256'u32
const MutationRate    = 2

var g_Rng:          Random.DefaultRng
var g_NumGenes:     u32
var g_Gen:          [2] Generation
var g_CurGen:       u32
var g_TotalWeight   = 0'f32
var g_BestScore     = Swag.F32.Max
var g_NumGen:       u32

struct Solution
{
    genes:  Array'u8
    score:  f32
    weight: f32
}

func genInit()
{
    g_NumGenes = g_Image0.width * g_Image0.height

    // Generate initial population
    var ranks: Array'u32
    loop indiv: PopulationCount
    {
        ranks.resize(cast(uint) g_NumGenes)
        loop i: g_NumGenes
            ranks[i] = i

        soluce := Memory.new'Solution()
        soluce.genes.resize(cast(uint) g_NumGenes * 3)
        g_Gen[0].all.add(soluce)

        cpt := 0'uint
        while ranks.count
        {
            idx := g_Rng.nextU32(0, cast(u32) ranks.count)
            idxP := ranks[idx]
            ranks.removeAt(cast(uint) idx)

            soluce.genes[cpt]   = g_Image0.pixels[idxP*3]
            soluce.genes[cpt+1] = g_Image0.pixels[(idxP*3)+1]
            soluce.genes[cpt+2] = g_Image0.pixels[(idxP*3)+2]

            cpt += 3
        }

        evaluateSolution(soluce)
    }

    // Sort solutions from best to worst
    g_Gen[0].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Placeholder for the next generation
    loop indiv: PopulationCount
    {
        soluce := Memory.new'Solution()
        soluce.genes.resize(cast(uint) g_NumGenes * 3)
        g_Gen[1].all.add(soluce)
    }
}

#[Swag.Safety("overflow", false)]
func colorDistance(rs, gs, bs, rd, gd, bd: u8)->f32
{
    rmean := cast(s32) ((rs + rd) / 2)
    r := cast(s32) rs - cast(s32) rd
    g := cast(s32) gs - cast(s32) gd
    b := cast(s32) bs - cast(s32) bd
    return Math.sqrt(cast(f32) ((((512+rmean)*r*r)>>8) + 4*g*g + (((767-rmean)*b*b)>>8)))
}

func evaluateSolution(soluce: *Solution)
{
    soluce.score = 0
    for idx := 0; idx < soluce.genes.count; idx += 3
    {
        rs := /*cast(f32)*/ soluce.genes[idx]
        gs := /*cast(f32)*/ soluce.genes[idx+1]
        bs := /*cast(f32)*/ soluce.genes[idx+2]

        rd := /*cast(f32)*/ g_Image1.pixels[idx]
        gd := /*cast(f32)*/ g_Image1.pixels[idx+1]
        bd := /*cast(f32)*/ g_Image1.pixels[idx+2]

        //dist := Math.abs(rs - rd) + Math.abs(gs - gd) + Math.abs(bs - bd)
        //dist /= 255*3

        dist := colorDistance(rs, gs, bs, rd, gd, bd) / 255.0

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
            src := g_Rng.nextU32(0, g_NumGenes) * 3
            dst := g_Rng.nextU32(0, g_NumGenes) * 3
            swap(child.genes.buffer + src, child.genes.buffer + dst)
            swap(child.genes.buffer + (src+1), child.genes.buffer + (dst+1))
            swap(child.genes.buffer + (src+2), child.genes.buffer + (dst+2))
        }
    }
}

func copyToNext(idxNext, idxCur: u32)
{
    nextGenIdx := (g_CurGen + 1) % 2
    Memory.copy(g_Gen[nextGenIdx].all[idxNext].genes.buffer, g_Gen[g_CurGen].all[idxCur].genes.buffer, cast(uint) g_NumGenes * 3)
    evaluateSolution(g_Gen[nextGenIdx].all[idxNext])
}

func genGo()->*Solution
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
    numChilds := 0'u32

    // Get from previous generation
    loop TakeBest
    {
        copyToNext(numChilds, numChilds)
        numChilds += 1
    }

    loop i: TakeWorst
    {
        copyToNext(numChilds, PopulationCount - i - 1)
        numChilds += 1
    }


    while numChilds < PopulationCount
    {
        // Selection
        father := pickToMate()
        mother := pickToMate(father)

        // Cross point
        crossPoint := cast(uint) g_Rng.nextU32(1, g_NumGenes - 2)
        crossPoint *= 3

        // Cross chromosomes
        child1 := g_Gen[nextGenIdx].all[numChilds]
        Memory.copy(child1.genes.buffer, father.genes.buffer, crossPoint)
        Memory.copy(child1.genes.buffer + crossPoint, mother.genes.buffer + crossPoint, (mother.genes.count - crossPoint))
        mutate(child1)
        evaluateSolution(child1)
        numChilds += 1
    }

    g_CurGen = (g_CurGen + 1) % 2

    // Sort solutions from best to worst
    g_Gen[g_CurGen].all.sort(@(x, y) {
        return x.score <=> y.score
    })

    // Returns the best score
    bestScore := g_Gen[g_CurGen].all[0].score
    if bestScore < g_BestScore
    {
        g_BestScore = bestScore
        @print(g_NumGen, ": ", bestScore, "\n")
    }

    //if g_NumGen == 10000
    //    Env.exit(0)

    return g_Gen[g_CurGen].all[0]
}