#include <Timer.hpp>

#include <iostream>
#include <unordered_map>
#include <memory>
#include <new>
#include <vector>
#include <algorithm>
#include <sstream>

cy::Timer_t timer;

using EdgeContainer = std::vector<int>;

struct Vertex {
	EdgeContainer E;
};

using VertexContainer = std::unordered_map<int, Vertex*>;

struct Graph {
	VertexContainer V;
        VertexContainer Vpred;
	bool init=true;

	void addEdge(int from, int to) {
		VertexContainer::iterator v;

		if ((v = V.find(from)) == V.end()) {
		v = V.insert(v, {from, new Vertex()});
		}
			if(init){
			std::vector<int>::iterator position = std::find(v->second->E.begin(), v->second->E.end(), to);
                		if (position == v->second->E.end())
				v->second->E.push_back(to);
			}
			else{
			v->second->E.push_back(to);
			}
		if ((v = Vpred.find(to)) == Vpred.end()) {
			v = Vpred.insert(v, {to, new Vertex()});
		}
		if(init){
			std::vector<int>::iterator position = std::find(v->second->E.begin(), v->second->E.end(), to);
                		if (position == v->second->E.end())
				v->second->E.push_back(from);
			}
			else{
			v->second->E.push_back(from);
			}
		}

		void removeEdge(int from, int to) {
		VertexContainer::iterator it;
		if ((it = V.find(from)) != V.end()) {
		std::vector<int>::iterator position = std::find(it->second->E.begin(), it->second->E.end(), to);
if (position != it->second->E.end()) 
    it->second->E.erase(position);
			}
		if ((it = Vpred.find(to)) != Vpred.end()) {
			std::vector<int>::iterator position = std::find(it->second->E.begin(), it->second->E.end(), from);
if (position != it->second->E.end()) 
    it->second->E.erase(position);
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
				int k = e;
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

int biBFS(int from, int to) {
		auto QF = std::vector<int>(); QF.reserve(64);
		auto QT = std::vector<int>(); QF.reserve(64);

		auto visitedF = std::unordered_map<int, int>();
		visitedF[from] = 0;
		size_t QFidx = 0;

		auto visitedT = std::unordered_map<int, int>();
		visitedT[to] = 0;
		size_t QTidx = 0;

		Vertex *v;
		QF.push_back(from);
		QT.push_back(to);
		while (QFidx < QF.size() && QTidx < QT.size()) {

			if ((QF.size()-QFidx) < (QT.size()-QTidx)) {
				size_t this_level = QF.size();
				for (;QFidx < this_level;) {
					int vid = QF[QFidx++];
					int newDepth = visitedF[vid] + 1;

					v = V[vid];
					for (const auto& e : v->E) {
						int k = e;
						auto it = visitedF.find(k);
						if (it == visitedF.end()) {
							visitedF.insert(it, {k, newDepth});
							// required to avoid having vertices that do not exist
							if (V.find(k) != V.end()) {
								QF.push_back(k);
							}
						}
						it = visitedT.find(k);
						if (it != visitedT.end()){
							//std::cerr << newDepth << ":" << from << ":" << to << " : " << k << std::endl;
							return it->second + newDepth;
						}
					}
				}
			} else {
				size_t this_level = QT.size();
				for (;QTidx < this_level;) {
					int vid = QT[QTidx++];
					int newDepth = visitedT[vid] + 1;

					v = Vpred[vid];
					for (const auto& e : v->E) {
						int k = e;
						auto it = visitedT.find(k);
						if (it == visitedT.end()) {
							visitedT.insert(it, {k, newDepth});
							// required to avoid having vertices that do not exist
							if (Vpred.find(k) != Vpred.end()) {
								QT.push_back(k);
							}
						}
						it = visitedF.find(k);
						if (it != visitedF.end()){
							return it->second + newDepth;
						}
					}
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
		return biBFS(from, to);
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
			std::cerr << "error" << std::endl;
			break;
		}
		if (line == "S") {
			G->init=true;
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
