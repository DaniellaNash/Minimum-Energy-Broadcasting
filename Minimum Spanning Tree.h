//
//  Minimum Spanning Tree.h
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

#ifndef Minimum_Spanning_Tree_h
#define Minimum_Spanning_Tree_h

#include <stdio.h>
#include<malloc/malloc.h>
#include<math.h>

#define MAX_TX_DISTANCE 999
#define MAX_ARRAY_SIZE 10

typedef struct Edge
{
    int **distance;//weight from 2 nodes
    int **label;//if edge exist between the two nodes
} edge;

int get_num_nodes(void);
void display (int display, int min_energy);
void *get_network_topology(int num_nodes, edge solution, int** temp);
int calculate_min_energy(int num_nodes, edge solution,int** temp );
void main_algorithm(int num_nodes, edge solution,int* visited);

#endif /* Minimum_Spanning_Tree_h */
