#include <Timer.hpp>

#include <iostream>
#include <unordered_map>
#include <memory>
#include <new>

cy::Timer_t timer;

using EdgeContainer = std::unordered_map<int, bool>;

struct Vertex {
	EdgeContainer E;
};

using VertexContainer = std::unordered_map<int, Vertex*>;

struct Graph {
	VertexContainer V;
	VertexContainer Vpred;

	void addEdge(int from, int to) {
		Vertex *v = V[from];
		if (v == nullptr) {
			v = new Vertex();
			V[from] = v;
		}
		v->E[to] = true;

		v = Vpred[to];
		if (v == nullptr) {
			v = new Vertex();
			Vpred[to] = v;
		}
		v->E[from] = true;
	}

	void removeEdge(int from, int to) {
		Vertex *v = V[from];
		if (v != nullptr) {
			v->E.erase(to);
		}

		v = Vpred[to];
		if (v != nullptr) {
			v->E.erase(from);
		}
	}
};

std::unique_ptr<Graph> readInitialGraph() {
	int from, to, r;
	char c;
	
	auto G = std::unique_ptr<Graph>(new Graph());

	for(;;) {
		if ((r = scanf("%d %d\n", &from, &to)) != 2) {
			r = scanf("%c", &c);
			//std::cerr << r << ":" << c << std::endl;
			break;
		}

		G->addEdge(from, to);
	}
	std::cout << "R";

	return std::move(G);
}


int main() {
	std::ios_base::sync_with_stdio(false);
    setvbuf(stdin, NULL, _IOFBF, 1<<20);

    auto start = timer.getChrono();
    std::unique_ptr<Graph> G = readInitialGraph();
    std::cerr << "readInitialGraph:: " << timer.getChrono(start) << std::endl;

}
