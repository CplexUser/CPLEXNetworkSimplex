#pragma once



#include <ilcplex/ilocplex.h>		// include cplex header files
#include <vector>
#include "typedefsandconsts.h"
struct NWARC_ {//This may be a GRAPH_ data structure in a larger project. Then, just pass a pointer to GRAPH_ object
	int from, to;
	int cost;
	int lb, ub;
	int arcno;
	//int orientation;//If orientation = 1, arc is from->to. If it is -1, arc is to->from
	NWARC_(int Fr, int To, int Cost, int Lb, int Ub, int Arcno)
		: from(Fr), to(To), cost(Cost), lb(Lb), ub(Ub), arcno(Arcno)
	{}
};


class NAC_ {//Network Algorithms via CPLEX, NAC_
public:
	void recovertree();
	void readprob(int nnn);
	void freememory();
	void solve();
	int solstat() { return Solstat; }
	double zval() { return Zval; }
	double ndslack(int n) { return Slack[n]; }
	double nddual(int n) { return Pi[n]; }
	double arcval(int m) { return X[m]; }
	double arcrc(int m) { return Dj[m]; }
	void chg_obj_for_sp(std::vector<NWARC> arcs);//Changes the objective function so that it is solving shortest path.
private:
	std::vector<std::vector<NWARC>> nodalarcs;//nodalarcs[nd][arc] 
	double *spcost;//Objective function array for shortest path.
	int *arcindices;//Also use these to extract cplex basis
	int *nodeindices;//Also use these to extract cplex basis
	int cnt, status;
	int *pred, *depth, *arcno, *flow, *orientation;//To recover the minimal basis tree.
	//orientation of 1 means that the arc is (pred[nd]->nd), -1 means arc is (nd->pred[nd])
	int nonodesencountered, *nodesencountered;
	int N, M;
	int S, T;//Source, Destination
	double Zval;
	double *Supply;
	int *Tail;
	int *Head;
	double *Obj;
	double *Ub;
	double *Lb;
	double *X;
	double *Dj;
	double *Pi;
	double *Slack;
	int Solstat;


	CPXENVptr	env;			// declare pointer to cplex environment
	CPXNETptr	net;				// declare pointer to network instance


	

};

