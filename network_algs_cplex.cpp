#include "network_algs_cplex.h"
#include "typedefsandconsts.h"
#include <ctime>

void NAC::solve() {
	status = CPXNETprimopt(env, net);
	status = CPXNETsolution(env, net, &Solstat, &Zval, X, Pi, Slack, Dj);
	
}


void NAC::recovertree() {
	if (Solstat == CPX_STAT_OPTIMAL) {
		//Now, construct the basis.
		status = CPXNETgetbase(env, net, arcindices, nodeindices);

		int currentnd;
		pred[S] = -1;
		depth[S] = 0;
		nonodesencountered = 1;
		nodesencountered[0] = S;
		int othernode;
		int ndindex = 0;
		while (1) {
			currentnd = nodesencountered[ndindex];
			for (int indx = 0; indx < nodalarcs[currentnd].size(); indx++) {
				if (arcindices[nodalarcs[currentnd][indx].arcno] == CPX_BASIC) {
					int ornt = -1;
					othernode = nodalarcs[currentnd][indx].from;
					if (nodalarcs[currentnd][indx].from == currentnd) {
						othernode = nodalarcs[currentnd][indx].to;
						ornt = 1;
						//Arc is currentnd -> othernode and currentnd is pred of othernode.
					}
					if (othernode != pred[currentnd]) {
						pred[othernode] = currentnd;
						depth[othernode] = depth[currentnd] + 1;
						arcno[othernode] = nodalarcs[currentnd][indx].arcno;
						flow[othernode] = X[nodalarcs[currentnd][indx].arcno];
						orientation[othernode] = ornt;
						nodesencountered[nonodesencountered] = othernode;
						nonodesencountered++;
						if (nonodesencountered == N)
							break;//get out of for loop

					}
				}
			}
			if (nonodesencountered == N)//get out of while loop
				break;
			ndindex++;
		}
	}
}





void NAC::readprob(int nnn) {
	clock_t time1 = clock();
	char fn[128];
	sprintf(fn, "data%d", nnn);
	strcat(fn, ".txt");
	FILE *fp = fopen(fn, "r");
	N = 500;
	int cst, fr, to;
	fscanf_s(fp, "%d %d", &S, &T);
	fscanf_s(fp, "%d", &M);
	env = CPXopenCPLEX(&status);
	net = CPXNETcreateprob(env, &status, NULL);

	Supply = new double[N];
	Tail = new int[M];
	Head = new int[M];
	Obj = new double[M];
	Ub = new double[M];
	Lb = new double[M];

	X = new double[M];
	Dj = new double[M];
	Pi = new double[N];
	Slack = new double[N];

	spcost = new double[M];
	arcindices = new int[M];
	nodeindices = new int[N];
	pred = new int[N];
	depth = new int[N];
	nodesencountered = new int[N];
	arcno = new int[N];
	flow = new int[N];
	orientation = new int[N];
	
	for (int n = 0; n < N; n++) {
		std::vector<NWARC> temp;
		nodalarcs.emplace_back(temp);
	}

	for (int arc = 0; arc < M; arc++) {
		fscanf_s(fp, "%d %d %d", &fr, &to, &cst);
		Tail[arc] = fr;
		Head[arc] = to;
		Lb[arc] = 0;
		Ub[arc] = CPX_INFBOUND;
		Obj[arc] = cst;
		//if (fr < to) 
			nodalarcs[fr].emplace_back(fr, to, cst, 0, CPX_INFBOUND, arc);
		//else 
			nodalarcs[to].emplace_back(fr, to, cst, 0, CPX_INFBOUND, arc);
	}
	for (int cnt = 0; cnt < N; cnt++) {
		Supply[cnt] = -1;//Every node has a demand of 1 unit
		nodeindices[cnt] = cnt;
	}
	Supply[S] = N - 1;//sth node has a supply of N - 1 units
	status = CPXNETchgobjsen(env, net, CPX_MIN);//Setup for general min cost flow problem.
	status = CPXNETaddnodes(env, net, N, Supply, NULL);
	status = CPXNETaddarcs(env, net, M, Tail, Head, Lb, Ub, Obj, NULL);
	clock_t time2 = clock();
	printf("Up until solving djsp\t\t\t%d\n", time2 - time1);
	clock_t time3 = clock();
	solve();
	clock_t time4 = clock();
	printf("solving djsp\t\t\t\t%d\t%d\t%d\n", time4 - time3, CPXNETgetphase1cnt(env, net), CPXNETgetitcnt(env, net));
	clock_t time5 = clock();
	recovertree();
	clock_t time6 = clock();
	printf("recovering tree\t\t\t\t%d\n", time6 - time5);
	//Now, printing path...//Specialized for dijkstra hence, does not need to use flow[] 
	clock_t time7 = clock();
	cst = 0;
	int currentnd = T;
	printf("%d ", currentnd);
	while (currentnd != S) {
		int prev = pred[currentnd];
		//arc on which flow is there is (prev->currentnd)
		printf("%d ", prev);
		cst += Obj[arcno[currentnd]];
		currentnd = prev;
	}
	std::cout << "Cost of " << cst << std::endl;
	clock_t time8 = clock();
	printf("printing\t\t\t\t%d\tTotaltime\t%d\n", time8 - time7, time8 - time1);
	printf("_____\n");
	//Doing modified problem.
	clock_t time9 = clock();
	for (int arc = 0; arc < M; arc++) {
		fscanf_s(fp, "%d %d %d", &fr, &to, &cst);
		arcindices[arc] = arc;
		Obj[arc] = cst;
	}
	status = CPXLNETchgobj(env, net, M, arcindices, Obj);
	clock_t time10 = clock();
	printf("Up until solving modified djsp\t\t%d\n", time10 - time9);
	clock_t time11 = clock();
	solve();
	clock_t time12 = clock();
	printf("resolving djsp\t\t\t\t%d\t%d\t%d\n", time12 - time11, CPXNETgetphase1cnt(env, net), CPXNETgetitcnt(env, net));
	clock_t time13 = clock();
	recovertree();
	clock_t time14 = clock();
	printf("recovering tree\t\t\t\t%d\n", time14 - time13);
	//Now, printing path...//Specialized for dijkstra hence, does not need to use flow[] 
	clock_t time15 = clock();
	cst = 0;
	currentnd = T;
	printf("%d ", currentnd);
	while (currentnd != S) {
		int prev = pred[currentnd];
		//arc on which flow is there is (prev->currentnd)
		printf("%d ", prev);
		cst += Obj[arcno[currentnd]];
		currentnd = prev;
	}
	std::cout << "Cost of " << cst << std::endl;
	clock_t time16 = clock();
	printf("printing\t\t\t\t%d\tTotaltime\t%d\n", time16 - time15, time16 - time9);
	printf("XXX Full Time \t\t\t\t\t\t\t\t\t %d \t\tXXX\n", time16 - time1);
	fclose(fp);
	freememory();
}





void NAC::freememory() {
	//CPLEX Related
	delete[] Supply;
	delete[] Tail;
	delete[] Head;
	delete[] Obj;
	delete[] Ub;
	delete[] Lb;

	delete[] X;
	delete[] Dj;
	delete[] Pi;
	delete[] Slack;

	delete[] spcost;
	delete[] arcindices;
	delete[] nodeindices;
	//Tree recovery related.
	delete[] pred;
	delete[] depth;
	delete[] nodesencountered;
	delete[] arcno;
	delete[] orientation;
	delete[] flow;


	status = CPXNETfreeprob(env, &net);
	status = CPXcloseCPLEX(&env);

}