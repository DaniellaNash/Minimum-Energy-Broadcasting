//
//  main.c
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

//
#include<malloc/malloc.h>
#include<stdio.h>
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

void main_algorithm(int num_nodes, edge solution,int* visited)
{
    int i,j,x,y;
    int num_edges = 1;
    int energy = 999, min_energy = 0;
    
    while(num_edges < num_nodes)//for each edge of MST (MST number of edges should = number of nodes -1)
    {
        for(i=1, energy =999; i<=num_nodes; i++)//for each node to node edge
            for(j=1; j<=num_nodes; j++)
                if(solution.distance[i][j]< energy)//check if the energy is not exceeding tx range i.e. edge exists
                    if(visited[i]!=0)            //and if the first node is visited
                    {
                        energy = solution.distance[i][j];  //then select that edge and compute the energy
                        x=i;
                        y=j;
                    }
        if(visited[x]==0 || visited[y]==0)//if either one of the node is not visited
        {
            printf("\n Edge %d:(%d %d) energy:%d",num_edges++,x,y,energy); //then
            min_energy += energy;
            visited[y]=1; //mark both nodes of the edge as visited
            solution.label[x][y] =1 ;//edge is used
            for(i=1; i<=num_nodes;i++)
                if(solution.distance[x][i]< energy)//all nodes within tx range used is also visited
                {
                    visited[i]=1;
                    solution.label[x][i]=1;
                }
        }
        solution.distance[x][y]=solution.distance[y][x]=MAX_TX_DISTANCE;//make sure this edge is not selected agai
    }
    
}

int calculate_min_energy(int num_nodes, edge solution,int** temp )
{
    int min_energy =0;
    int weight =0;
    int i,j;
    
    for(i=1; i<=num_nodes; i++)//for each node to node edge
    {
        weight = 0;
        for(j=1; j<=num_nodes; j++)
            if (solution.label[i][j]==1)
            {
                printf("\n%d -> %d: weight: %d",i,j,temp[i][j] );
                if(weight<temp[i][j])//find highest energy used per node
                {
                    weight = temp[i][j];
                }
            }
        
        min_energy += weight;
    }
    return min_energy;
}

void *get_network_topology(int num_nodes, edge solution, int** temp)
{
    int i,j;
    int distance;
    
    //ask for the distance needed of each edge the node takes and store them
    printf("\nEnter the network topology matrix:\n");
    for(i=1; i<=num_nodes; i++)
        for(j=1; j<=num_nodes; j++)
        {
            scanf("%d",&distance);
            
            if(distance !=0&&distance < MAX_TX_DISTANCE)
                solution.distance[i][j] = pow(distance,2);//convert distance to power
            else
                solution.distance[i][j] = MAX_TX_DISTANCE;//set non-existing(and not possible) edges to a large number (infinity) to prevent being selected
            
            solution.label[i][j]=0;//solution starts with no tree (edges)
        }
    
    for(i=1; i<=num_nodes; i++)//store distance data to temp
        for(j=1; j<=num_nodes; j++)
            temp[i][j]=solution.distance[i][j];
    return 0;
}

int get_num_nodes(void)
{
    int num_nodes;
    printf("\nEnter the number of nodes:");
    scanf("%d",&num_nodes);
    return num_nodes;
}

void display (int display, int min_energy)
{
    if(display ==0)
        printf("\n\n\n\n\n\nWelcome: Minimum Spanning Tree\n"); // Print introduction
    else
        printf("\n Minimun Energy found  %d",min_energy);
}

int main()
{
    int i;
    int* visited;//set all nodes to not visited
    int** temp;
    int min_energy=0,num_nodes;
    
    edge solution;
    
    FILE *fp;
    
    //creating new file with write mode
    fp=fopen("testSA.txt","w");
    if(fp==NULL)
    {
        printf("Error!");
        exit(1);
    }
    fprintf(fp,"%s %s\t%s\n" ,"Total","Energy","Solution");
    
    display(0,0);
    
    num_nodes = get_num_nodes();
    visited = (int *)malloc(num_nodes * sizeof(int));
    temp = malloc(num_nodes * sizeof(int*));//used to store original weight of edges
    solution.distance =malloc(num_nodes * sizeof(int*));//pointer to pointer for 2d array memory allocation
    solution.label =malloc(num_nodes * sizeof(int*));
    if(visited == NULL||solution.distance == NULL||solution.label == NULL||temp == NULL)
        fprintf(fp,"ERROR: Memory allocation failed!");
    for(i=1; i<=num_nodes; i++)
    {
        temp[i] = malloc(num_nodes * sizeof(int));
        solution.distance[i] = malloc(num_nodes * sizeof(int));
        solution.label[i] = malloc(num_nodes * sizeof(int));
        if(solution.distance[i] == NULL||solution.label[i] == NULL||temp[i] == NULL)
            fprintf(fp,"ERROR: Memory allocation failed!");
    }
    
    get_network_topology( num_nodes, solution,temp);
    visited[1]=1;//starting node
    main_algorithm( num_nodes,  solution, visited);
    min_energy = calculate_min_energy( num_nodes, solution, temp );
    display(1,min_energy);
    
    return 0;
}

// find highest energy of each node (not including its previous) and sum them for the total power
// node can reach all other nodes with equal or lower tx power
