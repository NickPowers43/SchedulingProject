// Learn more about F# at https://fsharp.org
// See the 'F# Tutorial' project for more help.

open System.IO

type ValType = int
type IdleTime = ValType
type JobLength = ValType
type ServerJobs = List<JobLength>
type AllJobs = List<ServerJobs>
type SyncPoints = List<ValType>
type BruteForceSolution = SyncPoints * IdleTime

let valInf: ValType = Core.int32.MaxValue

type Scenario =
    val syncPoints: List<ValType>
    val jobs: List<List<ValType>>
    val t: ValType
    val useT: bool
    new(syncPoints, jobs, t, useT) = {
        syncPoints = syncPoints;
        jobs = jobs;
        t = t;
        useT = useT;
    }

let loadScenario (path: string) : Scenario =
    printfn "Loading scenario at \"%s\"" path
    use stream = System.IO.File.OpenText path
    let version = stream.ReadLine()
    printfn "version: \"%s\"" version

    let readInt () = stream.ReadLine() |> int
    let readInts n = List.ofSeq (Seq.init n (fun i -> readInt ()))
    let readServer () = 
        let jobCount = readInt ()
        readInts jobCount
    
    let serverCount = readInt ()
    printfn "serverCount: \"%i\"" serverCount
    let jobs = List.ofSeq (Seq.init serverCount (fun i -> readServer ()))
         
    let syncPointCount = readInt ()
    printfn "syncPointCount: \"%i\"" syncPointCount
    let syncPoints = List.ofSeq (readInts syncPointCount)

    let t = readInt ()
    printfn "t: \"%i\"" t
    
    let useT = readInt ()
    printfn "useT: \"%i\"" useT

    printfn "syncPointCount: \"%i\"" syncPointCount
    new Scenario(syncPoints, jobs, t, useT > 0)
    
let saveScenario (scenario: Scenario) (path: string) : unit =

    printfn "saving scenario at \"%s\"" path
    use stream = System.IO.File.CreateText path

    stream.WriteLine "v3"

    stream.WriteLine (scenario.jobs.Length.ToString())
    for serverJobs in scenario.jobs do
        stream.WriteLine (serverJobs.Length.ToString())
        for job in serverJobs do
            stream.WriteLine (job.ToString())
            
    stream.WriteLine (scenario.syncPoints.Length.ToString())
    for syncPoint in scenario.syncPoints do
        stream.WriteLine (syncPoint.ToString())

    stream.WriteLine (scenario.t.ToString())
    stream.WriteLine ((if scenario.useT then 1 else 0).ToString())

    stream.Close()

let sumIdleTime (a: IdleTime) (b: IdleTime) : IdleTime =
    if a = valInf then valInf
    elif b = valInf then valInf
    else a + b

// Simulatneously computes idle time before "time" and computes remaining work to be done
let jobSplitter (time: ValType) (t: ValType) (useT: bool) : List<ValType> -> IdleTime * List<ValType> =
    fun jobs ->
        match jobs with
        | [] -> (0, [])
        | firstJob :: remainingJobs ->
            if firstJob <= time && remainingJobs.IsEmpty then (0, remainingJobs)
            elif firstJob <= time then (time - firstJob, remainingJobs)
            else (0, firstJob - time :: remainingJobs)

// Idle time and optimal syncPoints given a time for first sync point
let rec bestByTime (availableSyncPoints: int) (serverJobs: AllJobs) (t: ValType) (useT: bool) : ValType -> BruteForceSolution =
    if availableSyncPoints = 1
    then
        fun time ->
            let jobs = serverJobs |> List.map (jobSplitter time t useT)

            let jobIdleTime (jobs: ServerJobs) =
                if jobs.IsEmpty
                then t
                else (t - jobs.Head)

            let remainingIdleTime =
                jobs
                |> Seq.map (snd >> jobIdleTime >> (max 0))
                |> Seq.sum

            ([time], sumIdleTime (jobs |> List.sumBy fst) remainingIdleTime)
    else
        fun time ->
            let jobs = serverJobs |> List.map (jobSplitter time t useT)

            let optimalSyncPoints, remainingIdleTime = minimizeBruteForce (availableSyncPoints - 1) (jobs |> List.map snd) (t - time) useT

            let syncPoints = time :: (optimalSyncPoints |> List.map (fun p -> p + time))

            (syncPoints, sumIdleTime (jobs |> List.sumBy fst) remainingIdleTime)

and minimizeBruteForce (availableSyncPoints: int) (serverJobs: AllJobs) (t: ValType) (useT: bool) : BruteForceSolution =

    let lessThanT = (>) t

    let timeValues = 
        serverJobs
        |> Seq.filter (not << List.isEmpty)
        |> Seq.map (fun jobs -> jobs.Head)
        |> if useT then Seq.filter lessThanT else id
        |> Seq.toList
        |> List.distinct

    if timeValues.IsEmpty && availableSyncPoints = 0 then ([], 0)
    elif not timeValues.IsEmpty && availableSyncPoints = 0 then ([], valInf)
    elif timeValues.IsEmpty then ([], 0)
    else
        //try puting the first sync point a each time value. Find the one with least idle time
        timeValues
            |> List.map (bestByTime availableSyncPoints serverJobs t useT)
            |> List.minBy fst

let idleTime (scenario: Scenario) : IdleTime =

    let rec idleTimeForRemaining (serverJobs: AllJobs) (syncPoints: SyncPoints) : IdleTime =
        match syncPoints with
        | [] ->
            if serverJobs |> List.forall (not << List.isEmpty)
            then valInf
            else 0
        | time :: remaining ->
            let jobs = serverJobs |> List.map (jobSplitter time scenario.t scenario.useT)
            let remainingIdleTime = idleTimeForRemaining (jobs |> List.map snd) remaining
            sumIdleTime (jobs |> List.sumBy fst) remainingIdleTime

    idleTimeForRemaining scenario.jobs scenario.syncPoints

let runIdleTimeCalculator path = 
    let scenario = loadScenario path
    printfn "idle time: %i" (idleTime scenario)

let runBruteOptimizer path =

    let scenario = loadScenario path

    printfn "original syncPoints: %A" scenario.syncPoints

    let optimalSyncPoints, minimumIdleTime = minimizeBruteForce scenario.syncPoints.Length scenario.jobs scenario.t scenario.useT

    let optimalSolution = new Scenario(optimalSyncPoints, scenario.jobs, scenario.t, scenario.useT)

    saveScenario optimalSolution (String.concat "_" [path; "solution"])

    printfn "optimal syncPoints: %A" optimalSolution.syncPoints

[<EntryPoint>]
let main argv =

    runBruteOptimizer "random_scenario_a"
    
    let x = stdin.Read()
    
    0 // return an integer exit code
