//
//  Simulated Annealing.h
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

#ifndef Simulated_Annealing_h
#define Simulated_Annealing_h

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define MAX_NUM_NODE 100
#define MAX_ENERGY 0.0
#define MAX_DISTANCE 200
#define FALSE 0
#define TRUE 1

//each bit of the solution is an edge
// number of edges = number of nodes squared
//
//loops --> exist if they have the same ancestor (Dept first Search)
//If an unexplored edge leads to a node visited before, then the graph contains a cycle.
//NO LONGER EXISTS

//change weight to distance to power **do it in cost function
//store data
//
//make sure when you remove an edge
//remove before you add => no loops
//remove random node, this splits the tree into two branches
//add edge from each branch randomly
//find nodes at each branch using DFS


typedef struct Edge
{
    float weight;//distance
    int label;//solution
} edge;

//random number generator
int rand_binary(void);// 0 or 1
int rand_btw(int Min, int Max,int not_including);//random number between
float rand_float(void);//between 0 and 1

//tree traversal
void *dfs(int v, int num_nodes,edge** new_solution , int visited[]);//for identifying which nodes are in which branch
void *dfs_swap(int v, int num_nodes, edge** new_solution ,int visited[]);//for fixing node/edge ancestry

//main SA
float acceptance_prob(float T, float old_cost, float new_cost,edge** new_solution , edge** current_SA ,FILE *fp, int num_nodes);
float cost(int num_nodes, edge** new_solution );
void *print_output_file(int iteration, float old_cost, int num_nodes,edge** current_SA ,FILE *fp);

int temp_delete(edge** new_solution ,int n,int m);
int check_deadend(int num_nodes,edge** new_solution  ,int n,int m ,FILE *fp);
void *add_edge(int num_nodes,edge** new_solution , int n, int m, FILE *fp);
void *neighbour_solution(int num_nodes,edge** new_solution ,FILE *fp);

//initial SA setup
void *gen_init_solution(int num_edges, int num_nodes, int visited[],edge** initial_SA,edge** temp ,edge** current_SA );
void *get_network_topology( int num_nodes, edge **initial_SA, edge** temp ,int visited[]);
int *generate_rand_matrix(int num_nodes, edge **initial_SA, edge** temp , int visited[]);
int get_num_nodes(void);

#endif /* Simulated_Annealing_h */
