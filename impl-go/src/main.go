package main

import (
	"bufio"
	"fmt"
	"io"
	"os"
	"time"
)

func log(v ...interface{}) {
	fmt.Fprintf(os.Stderr, "%v", v)
}

func logln(v ...interface{}) {
	log(v)
	fmt.Fprintf(os.Stderr, "\n")
}

func timeReport(start time.Time, msg string) {
	total := time.Since(start)
	fmt.Fprintf(os.Stderr, "%s - Total time: %s\n", msg, total)
}

type Pair struct {
	A int
	B int
}

type Edge struct {
	To    int
	Valid bool
}

func buildEdge(to int) Edge {
	return Edge{
		To:    to,
		Valid: true,
	}
}

type EdgeContainer map[int]struct{}
type Vertex struct {
	E EdgeContainer
}

func buildVertex() *Vertex {
	return &Vertex{
		E: make(EdgeContainer),
	}
}

type Graph struct {
	V     map[int]*Vertex
	Vpred map[int]*Vertex
}

func buildGraph() Graph {
	return Graph{
		V:     make(map[int]*Vertex),
		Vpred: make(map[int]*Vertex),
	}
}

func addGraphEdge(G *Graph, from, to int) {
	var v *Vertex
	var ok bool

	if v, ok = G.V[from]; !ok {
		v = buildVertex()
		G.V[from] = v
	}
	v.E[to] = struct{}{}

	if v, ok = G.Vpred[to]; !ok {
		v = buildVertex()
		G.Vpred[to] = v
	}
	v.E[from] = struct{}{}
}

func addGraphEdges(G *Graph, batch []Pair) {
	for _, pair := range batch {
		addGraphEdge(G, pair.A, pair.B)
	}
}

func removeGraphEdge(G *Graph, from, to int) {
	var v *Vertex
	var ok bool

	if v, ok = G.V[from]; ok {
		delete(v.E, to)
	}
	if v, ok = G.Vpred[to]; ok {
		delete(v.E, from)
	}
}

type BFSHolder struct {
	Vid   int
	Depth int
}

func singleBFS(G *Graph, from, to int) int {
	Q := make([]BFSHolder, 0, 32)
	Qidx := 0
	visited := make(map[int]bool)
	visited[from] = true

	var v *Vertex
	Q = append(Q, BFSHolder{Vid: from, Depth: 0})
	for Qidx < len(Q) {
		ch := Q[Qidx]
		Qidx++

		v = G.V[ch.Vid]
		for k, _ := range v.E {
			if !visited[k] {
				if k == to {
					return ch.Depth + 1
				}
				if _, ok := G.V[k]; !ok {
					continue
				}
				Q = append(Q, BFSHolder{Vid: k, Depth: ch.Depth + 1})
				visited[k] = true
			}
		}
	}
	return -1
}

// biBFS does a bi-directional BFS to the graph G from source from to the target to.
// It returns the number of edges between the two vertices or -1 if no path exists.
func biBFS(G *Graph, from, to int) int {
	QF := make([]int, 0, 8)
	QT := make([]int, 0, 8)
	QFidx := 0
	QTidx := 0

	visitedF := make(map[int]int)
	visitedF[from] = 0

	visitedT := make(map[int]int)
	visitedT[to] = 0

	var v *Vertex
	QF = append(QF, from)
	QT = append(QT, to)
	for QFidx < len(QF) && QTidx < len(QT) {
		if len(QF)-QFidx < len(QT)-QTidx {
			levelIdx := len(QF)
			for ; QFidx < levelIdx; QFidx++ {
				vid := QF[QFidx]

				depth := visitedF[vid]

				v = G.V[vid]
				for k, _ := range v.E {
					if _, ok := visitedF[k]; !ok {
						visitedF[k] = depth + 1
						// required to avoid having vertices that do not exist
						if _, ok := G.V[k]; ok {
							QF = append(QF, k)
						}
					}
					if vdepth, ok := visitedT[k]; ok {
						return vdepth + visitedF[k]
					}
				}
			}
		} else {
			levelIdx := len(QT)
			for ; QTidx < levelIdx; QTidx++ {
				vid := QT[QTidx]

				depth := visitedT[vid]

				v = G.Vpred[vid]
				for k, _ := range v.E {
					if _, ok := visitedT[k]; !ok {
						visitedT[k] = depth + 1
						// required to avoid having vertices that do not exist
						if _, ok := G.Vpred[k]; ok {
							QT = append(QT, k)
						}
					}
					if vdepth, ok := visitedF[k]; ok {
						return visitedT[k] + vdepth
					}
				}
			}
		}

	}
	return -1
}

func BFS(G *Graph, from, to int) int {
	if from == to {
		return 0
	}
	if _, ok := G.V[from]; !ok {
		return -1
	}
	if _, ok := G.Vpred[to]; !ok {
		return -1
	}
	//return singleBFS(G, from, to)
	return biBFS(G, from, to)
}

////////////////////////////////////////////////////////////////////////

type EdgeBatchInput chan []Pair
type SignalChan chan bool

func prepareGraphBuilder(G *Graph) (SignalChan, EdgeBatchInput) {
	signal := make(SignalChan)
	input := make(EdgeBatchInput, 100)

	go func(signal SignalChan, input EdgeBatchInput) {
		defer timeReport(time.Now(), "Building initial graph finished.")
		logln("Builder started")

		for batch := range input {
			addGraphEdges(G, batch)
		}
		signal <- true
	}(signal, input)

	return signal, input
}

func readInitialGraph(r io.Reader) (Graph, error) {
	defer timeReport(time.Now(), "readInitialGraph:: finished.")

	G := buildGraph()

	signalChan, batchChan := prepareGraphBuilder(&G)

	var from, to int
	EDGES_BATCH_LIMIT := 10000
	edgesBatch := make([]Pair, 0, EDGES_BATCH_LIMIT)

	for {
		if _, err := fmt.Fscanf(r, "%d %d\n", &from, &to); err != nil {
			break
		}
		edgesBatch = append(edgesBatch, Pair{A: from, B: to})
		if len(edgesBatch) == EDGES_BATCH_LIMIT {
			batchChan <- edgesBatch
			edgesBatch = make([]Pair, 0, EDGES_BATCH_LIMIT)
		}
	}
	// Send the last batch
	if len(edgesBatch) > 0 {
		batchChan <- edgesBatch
	}
	close(batchChan)

	// Wait for the builder to finish
	<-signalChan
	logln("Total edges", len(edgesBatch))

	fmt.Println("R")

	return G, nil
}

type Operation struct {
	C    byte
	From int
	To   int
}
type ExecutionBatchInput chan []Operation

func prepareExecutionRunner(G *Graph) (SignalChan, ExecutionBatchInput) {
	signal := make(SignalChan)
	input := make(ExecutionBatchInput, 1000)

	go func(signal SignalChan, input ExecutionBatchInput) {
		for batch := range input {
			start := time.Now()
			for _, op := range batch {
				switch op.C {
				case 'Q':
					fmt.Println(BFS(G, op.From, op.To))
					break
				case 'A':
					addGraphEdge(G, op.From, op.To)
					break
				case 'D':
					removeGraphEdge(G, op.From, op.To)
					break
				}
			}
			timeReport(start, "batchExecution:: ")
		}
		signal <- true
	}(signal, input)

	return signal, input
}

func runExecution(r io.Reader, G *Graph) {
	defer timeReport(time.Now(), "runExecution:: finished.")

	EXECUTION_BATCH_LIMIT := 100

	signalChan, batchChan := prepareExecutionRunner(G)

	var c byte
	var from, to int

	inputBatch := make([]Operation, 0, EXECUTION_BATCH_LIMIT)

	for {
		if r, _ := fmt.Fscanf(r, "%c %d %d\n", &c, &from, &to); r < 3 {
			if r == 0 {
				break
			}
			if c == 'F' {
				//logln("F")
				if len(inputBatch) > 0 {
					batchChan <- inputBatch
					inputBatch = make([]Operation, 0, EXECUTION_BATCH_LIMIT)
				}
				continue
			}
		}
		inputBatch = append(inputBatch, Operation{C: c, From: from, To: to})
	}
	close(batchChan)

	// Wait for the runner to finish
	<-signalChan
}

func main() {

	G, err := readInitialGraph(bufio.NewReader(os.Stdin))
	if err != nil {
		log(os.Stderr, "Error: %v", err)
		return
	}
	log(len(G.V))

	runExecution(bufio.NewReader(os.Stdin), &G)

	/*
		for _, v := range G.V {
			log(len(v.E))
		}
	*/
}
