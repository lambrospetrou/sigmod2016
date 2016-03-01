#include <Timer.hpp>

#include "include/cpp_btree/btree_map.h"
#include "include/cpp_btree/btree_set.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <memory>
#include <new>
#include <sstream>
#include <string>

cy::Timer_t timer;

template<typename K, typename V>
using Map = btree::btree_map<K, V>;
template<typename K, typename V>
using Set = btree::btree_set<K, V>;

template<typename T>
using IntMap = btree::btree_map<int, T>;
//using IntMap = std::unordered_map<int, T>;

using IntSet = btree::btree_set<int>;

//using EdgeContainer = IntMap<bool>;
using EdgeContainer = IntSet;

struct Vertex {
	EdgeContainer E;
	int * componentId;
};

using VertexContainer = IntMap<Vertex*>;

struct Graph {
	VertexContainer V;
	VertexContainer Vpred;
	int componentsIds=-1;
	 int components[20000000] ;//= new int[200000]; 
	

	void addEdge(int from, int to) {
		VertexContainer::iterator vFrom =V.find(from);
		VertexContainer::iterator vTo=Vpred.find(to);
		VertexContainer::iterator vPredFrom=Vpred.find(from);
		VertexContainer::iterator vOrigTo=V.find(to);
		
		//from oxi mesa
		if(vFrom == V.end())
		{
			//both need to add
			if(vTo==Vpred.end())
			{
				//cboth nowhere
				if(vPredFrom==Vpred.end() && vOrigTo==V.end())
				{
					Vertex *v2=new Vertex();
					++componentsIds;
					//dixnw stin thesi
					////std::cerr<<"before "<<components[componentsIds];
					components[componentsIds]=componentsIds;
					
					v2->componentId=&components[componentsIds];//componentsIds;
					
					Vertex *v3=new Vertex();
					v3->componentId=&components[componentsIds];
					vFrom = V.insert(vFrom, {from, v2});
					vTo = Vpred.insert(vTo, {to, v3});
					
					//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v2->componentId)<<" eerwww "<<*(v3->componentId)<<std::endl;
					
				}
				//to is in the original from is nowhere
				else if(vPredFrom==Vpred.end() && vOrigTo!=V.end())
				{
					Vertex *v2=new Vertex();
					v2->componentId=vOrigTo->second->componentId;
					Vertex *v3=new Vertex();
					v3->componentId=vOrigTo->second->componentId;
					vFrom = V.insert(vFrom, {from, v2});
					vTo = Vpred.insert(vTo, {to, v3});
					//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v2->componentId)<<" lloooo "<<*(v3->componentId)<<std::endl;
				}
				else if(vPredFrom!=Vpred.end()&& vOrigTo==V.end())
				{
					Vertex *v2=new Vertex();
					v2->componentId=vPredFrom->second->componentId;
					Vertex *v3=new Vertex();
					v3->componentId=vPredFrom->second->componentId;
					vFrom = V.insert(vFrom, {from, v2});
					vTo = Vpred.insert(vTo, {to, v3});
					//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v2->componentId)<<" srrr "<<*(v3->componentId)<<std::endl;
				}
				//both stous anapodous (mporei na thelei allo solution dame)
				else{
					
					//prepei na pianoume tsino me ta pio lia pethkia level 1
					if(vOrigTo->second->componentId != vPredFrom->second->componentId)
					{*(vPredFrom->second->componentId)=*(vOrigTo->second->componentId);
						////std::cerr<<" case "<<*(vPredFrom->second->componentId)<<" tp "<<*(vOrigTo->second->componentId)<<std::endl;
						//components[*(vPredFrom->second->componentId)]=components[*(vOrigTo->second->componentId)];	
					}	
					
					//TODO:pou tha dixnoun meta?
					Vertex *v2=new Vertex();
					v2->componentId=vOrigTo->second->componentId;;//vOrigTo->second->componentId;
					Vertex *v3=new Vertex();
					v3->componentId=vOrigTo->second->componentId;
					vFrom = V.insert(vFrom, {from, v2});
					vTo = Vpred.insert(vTo, {to, v3});
					//std::cerr<<" from  "<<from<<" towww "<<to<<" "<<*(v2->componentId)<<" "<<*(v3->componentId)<<std::endl;
				}
				
			}
			//from den ine mesa ston kanoniko tou prepei na mpei alla o to ine
			else{
				//TO EN MESA
				
								if(vPredFrom==Vpred.end() && vOrigTo==V.end())
								{
													Vertex *v3=new Vertex();
													v3->componentId=vTo->second->componentId;
													vFrom = V.insert(vFrom, {from, v3});
													//std::cerr<<" from  "<<from<<" to "<<to<<" assse "<<*(v3->componentId)<<std::endl;
								}
								//to is in the original from is nowhere
								else if(vPredFrom==Vpred.end() && vOrigTo!=V.end())
								{
									Vertex *v3=new Vertex();
									v3->componentId=vTo->second->componentId;
									//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" 2fix originTo "<<*(vOrigTo->second->componentId)<<std::endl;
									*(vOrigTo->second->componentId)=*(vTo->second->componentId);
									vFrom = V.insert(vFrom, {from, v3});	
									
								}
								else if(vPredFrom!=Vpred.end()&& vOrigTo==V.end())
								{
									Vertex *v3=new Vertex();
									v3->componentId=vTo->second->componentId;
									//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" 3fix PredFrom "<<*(vPredFrom->second->componentId)<<std::endl;
									*(vPredFrom->second->componentId)=*(vTo->second->componentId);
									vFrom = V.insert(vFrom, {from, v3});		
								}
								//both stous anapodous (mporei na thelei allo solution dame)
								else{
									//prepei na pianoume tsino me ta pio lia pethkia level 1
									Vertex *v3=new Vertex();
									v3->componentId=vTo->second->componentId;
									//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" 4fix PredFrom "<<*(vPredFrom->second->componentId)<<" orig "<<*(vOrigTo->second->componentId)<<std::endl;
									*(vPredFrom->second->componentId)=*(vTo->second->componentId);
									*(vOrigTo->second->componentId)=*(vTo->second->componentId);
									vFrom = V.insert(vFrom, {from, v3});		
								}
			
	
			}
			
			
		}
		//from mesa
		else
		{
			//[prepei na mpei o TO
			if(vTo==Vpred.end())
						{
							
								if(vPredFrom==Vpred.end() && vOrigTo==V.end())
								{
					
									Vertex *v3=new Vertex();
									v3->componentId=vFrom->second->componentId;
									vTo = Vpred.insert(vTo, {to, v3});
									//std::cerr<<" from  "<<from<<" to "<<to<<" aa  "<<*(v3->componentId)<<std::endl;
								
								}
								else if(vPredFrom!=Vpred.end() && vOrigTo==V.end())
								{
									Vertex *v3=new Vertex();
									v3->componentId=vFrom->second->componentId;
									//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" 5fix predfrom " <<*(vPredFrom->second->componentId)<<std::endl;
									*(vPredFrom->second->componentId)=*(vFrom->second->componentId);
									vTo = Vpred.insert(vTo, {to, v3});		
									
								}
								else if(vPredFrom==Vpred.end() && vOrigTo!=V.end())
								{
									Vertex *v3=new Vertex();
																		v3->componentId=vFrom->second->componentId;
																		//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" fix orig" <<*(vOrigTo->second->componentId)<<std::endl;
																		*(vOrigTo->second->componentId)=*(vFrom->second->componentId);
																		vTo = Vpred.insert(vTo, {to, v3});		
								}
								else{
									Vertex *v3=new Vertex();
																		v3->componentId=vFrom->second->componentId;
																		//std::cerr<<" from  "<<from<<" to "<<to<<" "<<*(v3->componentId)<<" fix PredFrom "<<*(vPredFrom->second->componentId)<<" orig "<<*(vOrigTo->second->componentId)<<std::endl;
																		*(vPredFrom->second->componentId)=*(vFrom->second->componentId);
																		*(vOrigTo->second->componentId)=*(vFrom->second->componentId);
																		vTo = Vpred.insert(vTo, {to, v3});		
																		
								}
				
				
								
						}
			else{
				//both mesa
								
							if(*(vFrom->second->componentId)!=*(vTo->second->componentId))
								
							*(vFrom->second->componentId)=*(vTo->second->componentId);
//							if(vFrom->second->E.size()>0)
//															{
//																
//																for (const auto& e : vFrom->second->E)
//																{
//																	Vertex * v=Vpred[e];
//																	std::cerr<<"      inside "<<(*v->componentId)<<std::endl;
//																}
//															}
								//vFrom->second->componentId=vTo->second->componentId;
								
			}
			
			
		}
		
		
		
		
		
		vFrom->second->E.insert(to);
		vTo->second->E.insert(from);
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
	
	

	int biBFS(int from, int to) {
		auto QF = std::vector<int>(); QF.reserve(64);
		auto QT = std::vector<int>(); QF.reserve(64);

		auto visitedF = Map<int, int>();
		visitedF[from] = 0;
		size_t QFidx = 0;

		auto visitedT = Map<int, int>();
		visitedT[to] = 0;
		size_t QTidx = 0;

		Vertex *v;
		QF.push_back(from);
		QT.push_back(to);

		int vidF,vidT,fromN,toN;
					
		while (QFidx < QF.size() && QTidx < QT.size()) {

				vidF = QF[QFidx];
				v = V[vidF];
				fromN=v->E.size();
				
				vidT = QT[QTidx];
				v = Vpred[vidT];
				toN=v->E.size();

				if(fromN<=toN){
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
							////std::cerr << newDepth << ":" << from << ":" << to << " : " << k << std::endl;
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
		VertexContainer::iterator vFrom;
		VertexContainer::iterator vTo;
		if ((vFrom=V.find(from)) == V.end()) {
			return -1;
		}
		if ((vTo=Vpred.find(to)) == Vpred.end()) {
			return -1;
		}
		
		
		if(*(vFrom->second->componentId)!=*(vTo->second->componentId) )
		{//std::cerr<<"idfrom "<<from<<" "<<*(vFrom->second->componentId)<<" to "<<to<<" "<<*(vTo->second->componentId)<< " -1 actuall " << biBFS(from, to)<<std::endl;
			////std::cerr<<"-1"<<std::endl;
			return -1;
			
			
		}
		else{
			////std::cerr<<"idfrom "<<*(vFrom->second->componentId)<<" to "<<*(vTo->second->componentId)<< " -1 actuall " << biBFS(from, to)<<std::endl;	
		}

		
	
		//return singleBFS(from, to);
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
			//std::cerr << "error" << std::endl;
			break;
		}
		if (line == "S") {
			//std::cerr << "loadedInitialGraph:: " << timer.getChrono(start) << std::endl;
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

uint64_t tQ = 0, tA = 0, tD = 0;
void executeOperation(Graph &G, char c, int from, int to) {
	auto start = timer.getChrono();
	switch (c) {
	case 'Q':
		
		
		std::cout << G.BFS(from, to) << std::endl;
		tQ += timer.getChrono(start);
		break;
	case 'A':
		G.addEdge(from, to);
		tA += timer.getChrono(start);
		break;
	case 'D':
		G.removeEdge(from, to);
		tD += timer.getChrono(start);
		break;
	}
}

void runExecution(Graph &G) {
	int from, to;
	char c;
	std::string line;
	std::stringstream ss;

	uint64_t start = timer.getChrono();
	uint64_t lastEnd = 0;
	for(;;) {
		////std::cerr << "reading..." << std::endl;
		if (!std::getline(std::cin, line)) {
			break;
		}
		////std::cerr << "read: " << line << std::endl;
		if (line == "F") {
			uint64_t current = timer.getChrono(start) - lastEnd;
			lastEnd += current;
			std::cerr << "runExecutionBatch:: " << current << std::endl;
			continue;
		}
		ss.clear();
		ss.str(line);
		ss >> c >> from >> to;

		////std::cerr << "execute:: " << c << ":" << from << ":" << to << std::endl;
		executeOperation(G, c, from, to);
	}
}

int main() {
	std::ios_base::sync_with_stdio(false);
    setvbuf(stdin, NULL, _IOFBF, 1<<20);

    auto start = timer.getChrono();
    std::unique_ptr<Graph> G = readInitialGraph();
    //std::cerr << "readInitialGraph:: " << timer.getChrono(start) << std::endl;

    runExecution(*G);

    //std::cerr << "tQ: " << tQ << " tA: " << tA << " tD: " << tD << std::endl;
}