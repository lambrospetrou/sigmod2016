#include <Timer.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <new>
#include <sstream>
#include <string>

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
		VertexContainer::iterator v;
		if ((v = V.find(from)) == V.end()) {
			v = V.insert(v, {from, new Vertex()});
		}
		v->second->E[to] = true;

		if ((v = Vpred.find(to)) == Vpred.end()) {
			v = Vpred.insert(v, {to, new Vertex()});
		}
		v->second->E[from] = true;
	}

	void removeEdge(int from, int to) {
		VertexContainer::iterator it;
		if ((it = V.find(from)) != V.end()) {
			it->second->E.erase(to);
		}

		if ((it = Vpred.find(to)) != Vpred.end()) {
			it->second->E.erase(from);
		}
	}

	int singleBFS(int from, int to) {
		auto Q = std::vector<int>();
		Q.reserve(32);
		size_t Qidx = 0;
		auto visited = std::unordered_map<int, int>();
		visited[from] = 0;

		Vertex *v;
		Q.push_back(from);
		for (;Qidx < Q.size();) {
			int vid = Q[Qidx++];

			int newDepth = visited[vid] + 1;
			v = V[vid];
			for (const auto &e : v->E) {
				int k = e.first;
				if (visited.find(k) == visited.end()) {
					if (k == to) {
						return newDepth;
					}
					visited[k] = newDepth;
					if (V.find(k) == V.end()) {
						continue;
					}
					Q.push_back(k);
				}
			}
		}
		return -1;
	}

	int BFS(int from, int to) {
		if (from == to) {
			return 0;
		}
		if (V.find(from) == V.end()) {
			return -1;
		}
		if (Vpred.find(to) == Vpred.end()) {
			return -1;
		}
		return singleBFS(from, to);
		//return biBFS(G, from, to)
	}
};

std::unique_ptr<Graph> readInitialGraph() {
	int from, to;
	std::string line;
	std::stringstream ss;

	auto G = std::unique_ptr<Graph>(new Graph());
	auto start = timer.getChrono();

	for(;;) {
		if (!std::getline(std::cin, line)) {
			std::cerr << "error" << std::cin;
			break;
		}
		if (line == "S") {
			std::cerr << "loadedInitialGraph:: " << timer.getChrono(start) << std::endl;
			break;
		}
		ss.clear();
		ss.str(line);
		ss >> from >> to;

		G->addEdge(from, to);
	}
	std::cout << "R\n";

	return std::move(G);
}

void executeOperation(Graph &G, char c, int from, int to) {
	switch (c) {
	case 'Q':
		std::cout << G.BFS(from, to) << std::endl;
		break;
	case 'A':
		G.addEdge(from, to);
		break;
	case 'D':
		G.removeEdge(from, to);
		break;
	}
}

void runExecution(Graph &G) {
	int from, to;
	char c;
	std::string line;
	std::stringstream ss;

	auto start = timer.getChrono();	
	for(;;) {
		//std::cerr << "reading..." << std::endl;
		if (!std::getline(std::cin, line)) {
			break;
		}
		//std::cerr << "read: " << line << std::endl;
		if (line == "F") {
			std::cerr << "runExecutionBatch:: " << timer.getChrono(start) << std::endl;
			continue;
		}
		ss.clear();
		ss.str(line);
		ss >> c >> from >> to;

		//std::cerr << "execute:: " << c << ":" << from << ":" << to << std::endl;
		executeOperation(G, c, from, to);
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);
    setvbuf(stdin, NULL, _IOFBF, 1<<20);

    auto start = timer.getChrono();
    std::unique_ptr<Graph> G = readInitialGraph();
    std::cerr << "readInitialGraph:: " << timer.getChrono(start) << std::endl;

    runExecution(*G);
}
