//
//  Genetic Algorithm.h
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

#ifndef Genetic_Algorithm_h
#define Genetic_Algorithm_h

#include<stdio.h>         //to use the printf function
#include<stdlib.h>        //to use the rand function
#include<math.h>          //to use the pow function
#include<time.h>          //to use the time function

//define variables

//Limits on user input parameter values
#define MAX_CHROMO_SIZE             999 //the number of nodes
#define MIN_CHROMO_SIZE             2
#define MAX_POP_SIZE                500
#define MIN_POP_SIZE                4
#define MAX_MUTATION_RATE           1
#define MIN_MUTATION_RATE           0

//Standard values
#define TRUE                        1
#define FALSE                       0
#define MAX_ENERGY                  0
#define MAX_TX_DISTANCE             999
#define MAX_GENERATIONS             300

//creating the Edge structure
typedef struct Edge//per solution
{
    int label;//label
    float weight;//energy of each edge
} edge;

// creating the chrom structure
typedef struct Chrom
{
    edge **gene;
    float fitness_score;
    
} chrom;   // a chrom type

void introduction (void);
int generate_rand(int Min, int Max);
float generate_rand_float(void);
int get_restart(void);
int rand_binary(void);
int rand_btw(int Min, int Max,int not_including);

//get the parameter variables
int get_num_generation(void);
int get_population_size(void);
int get_chromosome_size(void);//the number of edges = number of nodes - 1
float get_mutation_rate(void);

//get the strategy
int get_selection_strategy(void);
int get_crossover_strategy(void);
int get_mutation_strategy(void);

//get initial data
void get_network_topology(int num_nodes, int pop_size, int visited[], chrom *temp, chrom *current_population);
void gen_init_pop(int num_nodes, int pop_size, int visited[], chrom *temp, chrom *current_population);

//calculate cost and fitness function
float cost(int num_nodes,int i,chrom *next_population);
float Objective_function(int num_nodes,int pop_size,int i,chrom *next_population);

void print_out_chroms(int num_nodes,int pop_size,chrom* next_population, int j, int best, int current_generation,FILE *fp);

//select strategy for each operator
void *do_selection_operator(int selection,int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);
void *do_crossover_operator(int crossover,int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);
void *do_mutation_operator(int mutation,int num_nodes,int pop_size,float mutation_rate, chrom* next_population,FILE *fp);

//selection strategies
void *selection_elitism_sort(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp,int show);
void *selection_rank(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);
void *selection_roulettewheel(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);
void *selection_tournament(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);

//crossover strategies
void *crossover_sameBranches(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);
void *dfs_swap2d(int v, int num_nodes,chrom* next_population,int x ,int **visited);
void *crossover_sBranches2(int num_nodes,int pop_size,int current_generation,chrom* next_population, chrom* temp,FILE *fp);
void *crossover_swapBranches(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp);

//mutation strategies
void *mutation_addRemove(int num_nodes,int pop_size,float mutation_rate, chrom* next_population,FILE *fp);
int temp_delete(chrom* next_population,int i ,int n,int m);
int check_deadend(int num_nodes,chrom* next_population,int i ,int n,int m ,FILE *fp);
void *dfs(int v, int num_nodes, chrom* next_population,int i ,int visited[]);
void *add_edge(int num_nodes,chrom* next_population, int j , int n, int m ,FILE *fp);
void *dfs_swap(int v, int num_nodes,chrom* next_population,int x ,int visited[]);
void *mutation_loopRemove(int num_nodes,int pop_size,int current_generation,float mutation_rate, chrom* next_population,FILE *fp);

#endif /* Genetic_Algorithm_h */
