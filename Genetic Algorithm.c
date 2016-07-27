//
//  Genetic Algorithm.c
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

#include "Genetic Algorithm.h"

int main()
{
    int i,j,x,y;
    int restart=0;
    int pop_size, num_nodes;
    float mutation_rate;
    int current_generation, num_gen;
    int selection, crossover, mutation;
    int best_fitness =-1, best_chrom;
    float sum_energy;
    int* visited;
    
    //system("MODE 100,25");//change size of cmd prompt
    
    FILE *fp;
    
    //creating new file with write mode
    fp=fopen("testGA.txt","w");
    if(fp==NULL)
    {
        printf("Error!");
        exit(1);
    }
    fprintf(fp,"%s %s\t%s %s\t\t%s %s\t%s\n" ,"Generation","number","Fitness",
            "Score","Total","energy","BINARY");
    
    srand(time(NULL));// refresh random values at every execution
    
    do
    {
        introduction (); //introduction plus the objective function declaration
        num_gen = get_num_generation();        // get the stopping criterion
        pop_size = get_population_size();
        num_nodes = get_chromosome_size();  //get number of edges = number of nodes - 1
        
        //*****************************************************UNCOMMENT THESE LATER***********************************
        //        mutation_rate = get_mutation_rate();
        //        selection = get_selection_strategy(); // get strategies for the operators
        crossover = get_crossover_strategy();
        //        mutation = get_mutation_strategy();
        //*************************************************************************************************************
        
        chrom* current_population;//initialize the population arrays
        chrom* next_population;
        chrom* temp;
        
        current_population = malloc(num_nodes * sizeof(current_population));
        temp = malloc(num_nodes * sizeof(temp));
        next_population = malloc(num_nodes * sizeof(next_population));
        visited = (int *)malloc(num_nodes * sizeof(int));
        
        if(current_population == NULL||next_population == NULL|| temp == NULL||visited==NULL)
            fprintf(fp,"ERROR23: Memory allocation failed!");
        for(x = 1; x <= num_nodes; x++)
        {
            current_population[x].gene = malloc(num_nodes * sizeof(edge*));
            next_population[x].gene = malloc(num_nodes * sizeof(edge*));
            temp[x].gene = malloc(num_nodes * sizeof(edge*));
            if(current_population[x].gene == NULL||next_population[x].gene == NULL||temp[x].gene == NULL)
                fprintf(fp,"ERROR24: Memory allocation failed!");
            
            for(y = 1; y <= num_nodes; y++)
            {
                current_population[x].gene[y] = malloc(num_nodes * sizeof(edge));
                next_population[x].gene[y] = malloc(num_nodes * sizeof(edge));
                temp[x].gene[y] = malloc(num_nodes * sizeof(edge));
                if(current_population[x].gene[y] == NULL||next_population[x].gene[y] == NULL||temp[x].gene[y] == NULL)
                    fprintf(fp,"ERROR25: Memory allocation failed!");
            }
        }
        //*************************************************************************************************************
        get_network_topology( num_nodes, pop_size, visited, temp, current_population);//get the matrix of node connections
        gen_init_pop(num_nodes, pop_size, visited, temp,current_population);//generate random population of solution
        //*************************************************************************************************************
        for(current_generation=1; current_generation<num_gen+1 ; current_generation++) // iterate for the number of generations wanted
        {
            printf("\n\n\n\tGeneration number: %d\n\n",current_generation); // print the iteration number
            
            for(j=1; j<=pop_size; j++)
                next_population[j]=current_population[j];//copy current population to next population in order to adjust it
            
            for(j=1; j<=pop_size; j++)//all next_population should be transfered from current_population before printing out as we calculate objective function here.
                print_out_chroms( num_nodes, pop_size,next_population,j,0,current_generation,fp);//print out chromosome and its fitness score
            
            best_fitness =-1;
            //            selection_elitism_sort(num_nodes, pop_size, current_generation, next_population,fp,0);//put population in order of highest fitness score
            for(j=1;j<=pop_size;j++)//find the chromosome with the highest fitness
            {
                if (best_fitness<next_population[j].fitness_score)
                {
                    best_fitness=next_population[j].fitness_score;
                    best_chrom = j;
                }
                
            }
            
            sum_energy = cost(num_nodes,best_chrom,next_population);
            fprintf(fp,"\n\t%i\t\t%.1f\t\t\t%.1f\t\t",current_generation,next_population[best_chrom].fitness_score,sum_energy);
            print_out_chroms(num_nodes, pop_size,next_population,best_chrom,1,current_generation,fp);//print out best chromosome
            
            //            for(j=1; j<=pop_size; j++)//all next_population should be transfered from current_population before printing out as we calculate objective function here.
            //            {
            //                printf("\n[%d]:",j);
            //                for(x=1;x<=num_nodes;x++)
            //                    for(y=1;y<=num_nodes;y++)
            //                        printf("%d",next_population[j].gene[x][y].label);
            //            }
            
            
            
            //*****************************************************UNCOMMENT THESE LATER***********************************
            //selection function
            //            do_selection_operator(selection, num_nodes, pop_size, current_generation, next_population,fp);
            //crossover function
            do_crossover_operator(crossover, num_nodes, pop_size, current_generation, next_population,fp);
            //            mutation function
            //            do_mutation_operator(mutation, num_nodes, pop_size, mutation_rate, next_population,fp);
            //*************************************************************************************************************
            for(i=1; i<=pop_size; i++)
                current_population[i]=next_population[i]; //copy the chromes of next population to current population
        }
        restart = get_restart();
    }
    while(restart==1);
    fclose(fp);//closing the file from write mode
    fflush(NULL);
    
    return 0;
}

void *crossover_sameBranches(int num_nodes,int pop_size,int current_generation,chrom* next_population,FILE *fp)//if doesnt work use MST
{
    int i,j,x,y;
    int num_edges =1;
    int n_new, m_new;//join broken branch by edge n_new and m_new
    int **visited;
    int sameBranchDetected = FALSE;
    chrom* child;
    
    printf("\n\n\tSame Branches crossover:");
    
    //allocate memory of child
    child = malloc(num_nodes * sizeof(child));
    visited = malloc(num_nodes * sizeof(int*));
    if(child == NULL||visited==NULL)
        fprintf(fp,"ERROR1: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        child[x].gene = malloc(num_nodes * sizeof(edge*));
        visited[x] = malloc(num_nodes * sizeof(int));
        if(child[x].gene == NULL||visited[x]==NULL)
            fprintf(fp,"ERROR2: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            child[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(child[x].gene[y] == NULL)
                fprintf(fp,"ERROR3: Memory allocation failed!");
        }
    }
    
    for(j=1;j<=pop_size;j++)//make storage for new solutions
    {
        //        printf("\n\t**Chromosome [%d]:",j);
        for(x=1;x<=num_nodes;x++)
            for(y=1;y<=num_nodes;y++)
            {
                
                child[j].gene[x][y].label =0;
                //                printf("%d",next_population[j].gene[x][y].label);
            }
    }
    
    for(j=1; j<=pop_size; j+=2) //each parent
    {
        sameBranchDetected = FALSE;
        for(x=1; x<=num_nodes; x++)//check if edge exists in both
            for(y=1;y<=num_nodes;y++)
                if (next_population[j].gene[x][y].label==1||next_population[j].gene[y][x].label==1)
                {
                    //                    printf("\nchrom %d (%d %d):label:%d.%d",j,x,y,next_population[j].gene[x][y].label==1,next_population[j].gene[y][x].label==1);
                    if (next_population[j+1].gene[x][y].label==1||next_population[j+1].gene[y][x].label==1)
                    {
                        //                        printf("\nchrom %d (%d %d):label:%d.%d",j+1,x,y,next_population[j].gene[x][y].label==1,next_population[j].gene[y][x].label==1);
                        child[j].gene[x][y].label =1;//set label to 1 for one directions for now
                        child[j+1].gene[x][y].label =1;//do this for both children - both child will differ when the 'broken branches' are joined together randomly
                        //                        printf("\n[%d %d] same edge (%d %d), label%d",j,j+1,x,y,next_population[j].gene[x][y].label);
                        sameBranchDetected = TRUE;
                    }
                }
        for(x=1; x<=num_nodes; x++)//delete duplicates
            for(y=1;y<=num_nodes;y++)
                if(child[j].gene[x][y].label==1)
                {
                    child[j].gene[y][x].label =0;
                    child[j+1].gene[y][x].label =0;
                }
        
        if(sameBranchDetected==FALSE)
            printf("\nno branch the same btw %d and %d",j,j+1);
    }
    
    
    //now having copied over branches that are similar to both parents
    //dfs from node 1, then join a random non visited edge to the tree
    //set all connected nodes from that edge to visited
    //dfs again from 1 (dfs swap along the way) and do as previous
    //until all nodes are connected - i.e. is a solution
    for(i=1;i<=pop_size;i++) //do this per solution
    {
        num_edges = 1;
    add_again:
        for(x=1; x<=num_nodes; x++)//create storage space to list visited nodes
            visited[i][x]=0;
        dfs_swap2d(1,num_nodes,next_population,i, visited);//visited now has the list of nodes connected to 1 and nodes are set in the correct order
        
        for(x=1; x<=num_nodes; x++)//check if all nodes are visited
            if(visited[i][x]==1)
                num_edges++;
        
        if(num_edges >= (num_nodes-1))
        {
            goto done;
            printf("\nall the same branches");
        }
        
        n_new = rand_btw(1,num_nodes,0);//select two random nodes, n has to be connected to the source node
        m_new = rand_btw(1,num_nodes,n_new);
        
        while(visited[i][n_new]==1 && visited[i][m_new]!=1)//make sure random node is in the correct branch
        {
        here2:
            n_new = rand_btw(1,num_nodes,0);
            m_new = rand_btw(1,num_nodes,n_new);
        }
        while(next_population[i].gene[n_new][m_new].weight ==MAX_ENERGY)//while edge is not possible, find another edge until a possible edge is found
        {
            n_new = rand_btw(1,num_nodes,0);//select two random nodes
            m_new = rand_btw(1,num_nodes,n_new);
            
            while(visited[i][n_new]==1 && visited[i][m_new]!=1)//make sure random node is in the correct branch
            {
                goto here2;
            }
        }
        
        printf("\n [%d] Add Edge (%d %d)",i,n_new,m_new);
        child[i].gene[n_new][m_new].label= 1;//add this random edge to the solution
        num_edges++;
    done:
        printf("\n*****num_edges = %d",num_edges);
        while(num_edges<num_nodes)//num_edges = num_nodes -1
        {
            printf("****num_edges:%d,num_nodes:%d",num_edges,num_nodes);
            goto add_again;
        }
    }
    
    for(i=1; i<pop_size+1; i++)//print out
    {
        for(x=1; x<=num_nodes; x++)//check if edge exists in both
            for(y=1;y<=num_nodes;y++)
                next_population[i].gene[x][y].label=child[i].gene[x][y].label;
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    return 0;
}

void *crossover_sBranches2(int num_nodes,int pop_size,int current_generation,chrom* next_population, chrom* temp,FILE *fp)//what happens if no common edge?
{
    int i,j,x,y,n;
    int num_edges =0;
    int *visited;
    float rand;
    chrom* child;
    
    printf("\n\n\tSame Branches crossover:");
    
    //allocate memory of child
    child = malloc(num_nodes * sizeof(child));
    visited = (int *)malloc(num_nodes * sizeof(int));
    if(child == NULL||visited==NULL)
        fprintf(fp,"ERROR4: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        child[x].gene = malloc(num_nodes * sizeof(edge*));
        if(child[x].gene == NULL)
            fprintf(fp,"ERROR5: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            child[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(child[x].gene[y] == NULL)
                fprintf(fp,"ERROR6: Memory allocation failed!");
        }
    }
    
    for(j=1; j<=pop_size; j+=2) //each parent
        for(x=1; x<=num_nodes; x++)//check if edge exists in both
            for(y=1;y<=num_nodes;y++)
                if ((next_population[j].gene[x][y].label==1||next_population[j].gene[y][x].label==1)&&(next_population[j+1].gene[x][y].label==1||next_population[j+1].gene[y][x].label==1))
                {
                    child[j].gene[x][y].label =1;//set label to 1 for one directions for now
                    child[j+1].gene[x][y].label =1;//do this for both children - both child will differ when the 'broken branches' are joined together randomly
                }
    //now having copied over branches that are similar to both parents
    //dfs from node 1, then join a random non visited edge to the tree
    //set all connected nodes from that edge to visited
    //dfs again from 1 (dfs swap along the way) and do as previous
    //until all nodes are connected - i.e. is a solution
    for(i=1; i<=pop_size; i++) //for each chromosome
    {
        for(j=1; j<=num_nodes; j++) //for each solution clear the visited list
            visited[j]=0;
        
        num_edges =1;
        printf("\n\n\tPopulation: %d ",i);
        visited[1]=1;//set node one as the source node
        
        while(num_edges < num_nodes)//have required #edges = #node-1
        {
            for(x=1; x<=num_nodes; x++)
                for(y=1; y<=num_nodes; y++)//for all edges
                {
                    if((child[i].gene[x][y].label==1)||(child[i].gene[y][x].label==1))
                    {
                        visited[x]=1;
                        visited [y] =1;
                        child[i].gene[x][y].weight=child[i].gene[y][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                    }
                    if(visited[x]!=0)//if node is visited
                        if(child[i].gene[x][y].weight!=MAX_ENERGY) //if edge is possible
                            if(visited[y]==0)//if node is not yet visited
                            {
                                rand = rand_binary();
                                child[i].gene[x][y].label=rand;//randomly add an edge
                                if (rand == 1)
                                {
                                    visited [y] = 1;
                                    printf("\n\t Edge1 %d:(%d %d) energy:%.1f, label:%d",num_edges++,x,y,child[i].gene[x][y].weight,child[i].gene[x][y].label);
                                    for(n=1; n<=num_nodes; n++)
                                        if(n!=y)//find different edge connected to node x
                                            if (child[i].gene[x][n].weight != MAX_ENERGY)//if edge is possible
                                                if(child[i].gene[x][n].weight<= child[i].gene[x][y].weight)// node can reach all other nodes with equal or lower tx power
                                                    if(visited[n]==0)//if node is not yet visited
                                                    {
                                                        visited[n]=1;
                                                        child[i].gene[x][n].label = 1;
                                                        printf("\n\t Edge2 %d:(%d %d) energy:%.1f, label:%d",num_edges++,x,n,child[i].gene[x][n].weight,child[i].gene[x][n].label);
                                                        child[i].gene[x][n].weight=child[i].gene[n][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                                                    }
                                    child[i].gene[x][y].weight=child[i].gene[y][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                                }
                            }
                }
        }
    }
    
    //transfer original weight back
    for(i=1; i<=pop_size; i++)
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)
                child[i].gene[x][y].weight = temp[i].gene[x][y].weight;
    
    //get everything in the right order
    for(i=1; i<=pop_size; i++)
        dfs_swap(1,num_nodes,child,i,visited);
    
    for(i=1; i<pop_size+1; i++)//print out
    {
        next_population[i]=child[i];
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    return 0;
}

void *crossover_swapBranches(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)//what happens if no common edge?
{
    int i,j,x,y;
    int source;
    int *branch1, *branch2, *branch3,*branch4, *visited;
    chrom* children;
    int same_edgefound =0;
    
    printf("\n\n\tSwap Branches crossover:");
    
    //allocate memory of children and branchs
    children = malloc(num_nodes * sizeof(children));
    branch1 = (int *)malloc(num_nodes * sizeof(int));
    branch2 = (int *)malloc(num_nodes * sizeof(int));
    branch3 = (int *)malloc(num_nodes * sizeof(int));
    branch4 = (int *)malloc(num_nodes * sizeof(int));
    visited = (int *)malloc(num_nodes * sizeof(int));
    if(children == NULL||branch1==NULL||branch2==NULL||branch3==NULL||branch4==NULL||visited==NULL)
        fprintf(fp,"ERROR7: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        children[x].gene = malloc(num_nodes * sizeof(edge*));
        if(children[x].gene == NULL)
            fprintf(fp,"ERROR8: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            children[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(children[x].gene[y] == NULL)
                fprintf(fp,"ERROR9: Memory allocation failed!");
        }
    }
    
    for(j=1; j<=pop_size; j+=2) //each parent
    {
        for(x=1; x<=num_nodes; x++)//check if edge exists in both
            for(y=1;y<=num_nodes;y++)
                if ((children[j].gene[x][y].label==1||children[j].gene[y][x].label==1)&&(children[j+1].gene[x][y].label==1||children[j+1].gene[y][x].label==1))
                {
                    if(same_edgefound==0)//while no common edge is found
                    {
                        same_edgefound = 1;//find common edge between two parents
                        children[j].gene[x][y].label=0;//temporarily delete said edge
                        children[j].gene[y][x].label=0;
                        children[j+1].gene[x][y].label=0;//temporarily delete said edge for both children
                        children[j+1].gene[y][x].label=0;
                    }
                }
        same_edgefound = 0;//reset back to zero for next set of parents
    }
    
    for(j=1; j<=pop_size; j+=2) //each parent
    {
        for(x=1; x<=num_nodes; x++)//create storage space to list nodes in each branch
        {
            branch1[x]=0;
            branch2[x]=0;
            branch3[x]=0;
            branch4[x]=0;
        }
        dfs(x,num_nodes,children,j,branch1);//for each child
        dfs(y,num_nodes,children,j,branch2);
        dfs(x,num_nodes,children,j+1,branch3);//for each child
        dfs(y,num_nodes,children,j+1,branch4);
        
        for(x=1;x<=num_nodes;x++)//find which branch has the source node
        {
            if(branch1[1]==1)//for the fist child
                source =1;
            else if(branch2[1]==1)
                source = 2;
            else if(branch3[1]==1)//for the second child
                source = 3;
            else
                source = 4;
        }
        
        for(x=1; x<=num_nodes; x++)//create storage space to list nodes in each branch
        {
            branch1[x]=0;
            branch2[x]=0;
            branch3[x]=0;
            branch4[x]=0;
            visited[x]=0;
        }
        if(source ==1)//get the branch with with the source node in the correct order
            dfs_swap(1,num_nodes,children,j,branch1);
        if(source ==2)
            dfs_swap(1,num_nodes,children,j,branch2);
        if(source ==3)
            dfs_swap(1,num_nodes,children,j+1,branch3);
        if(source ==4)
            dfs_swap(1,num_nodes,children,j+1,branch4);
        
        //rejoin the edge between the two branches
        children[j].gene[x][y].label=1;//temporarily add said edge
        children[j+1].gene[x][y].label=1;
        for(x=1; x<=num_nodes; x++)//create storage space to list nodes in each branch
        {
            branch1[x]=0;
            branch2[x]=0;
        }
        dfs_swap(1,num_nodes,children,j,branch1);
        dfs_swap(1,num_nodes,children,j+1,branch2);
        
        //delete loop
        //rejoin via MST
    }
    
    for(i=1; i<pop_size+1; i++)//print out
    {
        next_population[i]=children[i];
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    
    return 0;
}

void *dfs_swap2d(int v, int num_nodes,chrom* next_population,int x ,int **visited)
{
    int i;
    
    visited[x][v]=1;//mark the node as visited
    
    for (i=1;i<=num_nodes;i++)//for all edges
    {
        //  if (i!=x &&v !=y)//not including newly added edge
        if(!visited[x][i]==1)//with nodes not yet visited
        {
            if(next_population[x].gene[v][i].label !=1)//swap if wrong order
                if(next_population[x].gene[i][v].label ==1)//check is possible
                {
                    next_population[x].gene[v][i].label = 1;
                    next_population[x].gene[i][v].label = 0;
                    printf("\n switch1 order of edge (%d %d) to (%d %d)",i,v,v,i);
                }
            if(next_population[x].gene[v][i].label ==1)
            {
                dfs_swap2d(i, num_nodes, next_population,x,visited);
            }
        }
    }
    return 0;
}
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
void *mutation_loopRemove(int num_nodes,int pop_size,int current_generation,float mutation_rate, chrom* next_population,FILE *fp)
{
    // add random edge
    // if dfs from each node
    //if node is visited 2x by a node ==loop
    //if no loop go back to start
    
    //now loop is made
    //remove random edge from that dfs visited list;
    int i,j,x;
    float random;
    int *visited;
    int n_new,m_new;
    
    visited = (int *)malloc(num_nodes * sizeof(int));
    if(visited==NULL)
        fprintf(fp,"ERROR10: Memory allocation failed!");
    
    printf("\n\n\tAdd/Remove mutation:");
    
    for(j=1;j<=pop_size;j++)
    {
        random = generate_rand_float();//get value between 0 and 1
        if (random<mutation_rate) //mutation rate of 1%
        {
            //back:
            n_new = rand_btw(1,num_nodes,0);//select two random nodes
            m_new = rand_btw(1,num_nodes,n_new);
            
            while(next_population[j].gene[n_new][m_new].label !=1||next_population[j].gene[n_new][m_new].weight ==MAX_ENERGY)//make sure that edge is not already in the solution
            {//while edge is not possible, find another edge until a possible edge is found
                n_new = rand_btw(1,num_nodes,0);//select two random nodes
                m_new = rand_btw(1,num_nodes,n_new);
            }
            printf("\n Add Edge1 (%d %d)",n_new,m_new);
            next_population[j].gene[n_new][m_new].label= 1;//add this random edge to the solution
            
            for(x=1;x<=num_nodes;x++)
                visited[x]=0;
            
            dfs_swap(1,num_nodes,next_population,j,visited);//to get in the right order
            
            for(x=1;x<=num_nodes;x++)
            {
                visited[x]=0;
                dfs(x,num_nodes,next_population,j,visited);
                //find if visited 2x
                //else
                //                {
                //                    next_population[j].gene[n_new][m_new].label= 0;//revert back as there are no loops (THIS IS IMMPOSSIBLE - there will always be a looped formed)
                //                    goto back;
                //                }
            }
            
            for(i=1; i<pop_size+1; i++)//print out
                print_out_chroms(num_nodes,pop_size,next_population,i,0,0,fp);
        }
        else
            printf(" N/A");
    }
    return 0;
}

void *mutation_addRemove(int num_nodes,int pop_size,float mutation_rate, chrom* next_population,FILE *fp)
{
    int set;
    int n,m,i;
    int edgeRemoved =FALSE;
    int deadend =FALSE;
    float random;
    
    printf("\n\n\tAdd/Remove mutation:");
    
    for(i=1; i<pop_size+1; i++)//for each chromosome solution
    {
        random = generate_rand_float();//get value between 0 and 1
        if (random<mutation_rate) //mutation rate
        {
            edgeRemoved = FALSE;//reset
            //remove a random edge
            while (edgeRemoved == FALSE)//remove a random edge
            {
            redo:
                n = rand_btw(1,num_nodes,0);//select two random nodes to remove the edge from
                m = rand_btw(1,num_nodes,n);//node [0]=n, m =m
                
                while(next_population[i].gene[n][m].label ==0)//make sure that the edge is in the current solution.//while edge is not in solution, find another edge until solution edge is found. Note, if edge is in current solution then edge is also possible i.e. has a weight>0
                {
                    n = rand_btw(1,num_nodes,0);
                    m = rand_btw(1,num_nodes,n);//prevent picking the same node
                }
                
                //temporarily delete edge
                set = temp_delete( next_population,i, n, m);
                deadend = check_deadend(num_nodes,next_population,i,n,m,fp);
                
                while(deadend ==TRUE)//while delete branch is a deadend
                {
                    if(set==0)
                        next_population[i].gene[m][n].label =1;
                    else
                        next_population[i].gene[n][m].label =1;   //undelete the branch
                    goto redo;
                }
                printf("\n\tRemove Edge (%d %d)",n,m);
                next_population[i].gene[n][m].label =next_population[i].gene[m][n].label= 0;//remove this random edge from the solution
                edgeRemoved = TRUE;//this separates the tree into two branches
            }
            
            //connect the two branches back together to make the solution tree
            add_edge( num_nodes, next_population,i,n, m,fp);
            print_out_chroms(num_nodes,pop_size,next_population,i,0,0,fp);
        }
        else
            printf(" \nN/A");
    }
    return 0;
}

int temp_delete(chrom* next_population,int i ,int n,int m)
{
    int set;
    if (next_population[i].gene[m][n].label ==1)
    {
        set = 0;
        next_population[i].gene[m][n].label= 0;
    }
    if (next_population[i].gene[n][m].label ==1)
    {
        set=1;
        next_population[i].gene[n][m].label= 0;
    }
    return set;
    
}

int check_deadend(int num_nodes,chrom* next_population,int i ,int n,int m ,FILE *fp)
{
    int count1=0;//keep count of how many potential edges can be made of each branch
    int count2=0;
    int x,y;
    int deadend = FALSE;
    int *branchN,*branchM;
    
    branchN = (int *)malloc(num_nodes * sizeof(int));
    branchM = (int *)malloc(num_nodes * sizeof(int));
    if (branchM==NULL||branchN==NULL)
        fprintf(fp,"\nError allocating memory!\n"); //print an error message
    
    //delete edge only if it is not a dead end
    for(x=1; x<=num_nodes; x++)//create storage space to list nodes in each branch
    {
        branchN[x]=0;
        branchM[x]=0;
    }
    
    dfs(n,num_nodes,next_population,i,branchN);
    dfs(m,num_nodes,next_population,i,branchM); //find nodes in n branch using DFS
    //there has to be more than one possible edge for branch
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++) //for each edge from each branch
        {
            if(branchN[x]==1&&branchM[y]==1)//to the other branch
                if(next_population[i].gene[x][y].weight !=MAX_ENERGY)//that is possible
                    count1 ++;
            if (branchM[x]==1&&branchN[y]==1)
                if(next_population[i].gene[x][y].weight !=MAX_ENERGY)//that is possible
                    count2 ++;
        }
    if (count1<2||count2<2)
        deadend = TRUE;
    return deadend;
}

void *dfs(int v, int num_nodes, chrom* next_population,int i ,int visited[])
{
    int x;
    
    visited[v]=1;
    for (x=1; x<=num_nodes; x++)
        if(next_population[i].gene[v][x].label==1||next_population[i].gene[x][v].label==1)
            if (!visited[x]==1) //if not yet visited
                dfs(x, num_nodes, next_population,i,visited);
    return 0;
}

void *add_edge(int num_nodes,chrom* next_population, int j , int n, int m ,FILE *fp)
{
    int x,i;
    int source;
    int n_new,m_new;
    int edgeAdded=FALSE;
    int *visited;
    int *branchN,*branchM;
    
    branchN = (int *)malloc(num_nodes * sizeof(int));
    branchM = (int *)malloc(num_nodes * sizeof(int));
    visited = (int *)malloc(num_nodes * sizeof(int));
    if (branchM==NULL||branchN==NULL||visited==NULL)
        fprintf(fp,"\nError allocating memory!\n"); //print an error message
    
    //DFS function
    //find nodes in n branch using DFS
    while (edgeAdded==FALSE)//add edge between the two branches to make a complete tree
    {
        source = 0;
        for(x=1; x<=num_nodes; x++)//create storage space to list nodes in each branch
        {
            branchN[x]=0;
            branchM[x]=0;
            visited[x]=0;
        }
        dfs(n,num_nodes,next_population,j,branchN);
        dfs(m,num_nodes,next_population,j,branchM); //find nodes in n and m branch using DFS
        
        // printf("\n node N is connected to node:");
        for (i=1; i<=num_nodes; i++)
            if(branchN[i]==1)
            {
                //  printf(" %d",i);//print nodes connected to n
                if (i==1)
                    source = 0;//make note of which branch the source node is in
            }
        // printf("\n node M is connected to node: ");
        for (i=1; i<=num_nodes; i++)
            if(branchM[i]==1)
            {
                //  printf("% d",i);//print nodes connected to m
                if (i==1)
                    source=1;//make note of which branch the source node is in
                
            }
        
        //select random node from each branch branchM and branchN
        n_new = rand_btw(1,num_nodes,0);//select two random nodes
        m_new = rand_btw(1,num_nodes,n_new);
        
        while(branchN[n_new]==0)//make sure random node is in the correct branch
            n_new = rand_btw(1,num_nodes,0);
        while(branchM[m_new]==0)
            m_new = rand_btw(1,num_nodes,0);
        
        while((n==n_new && m==m_new)|| (n==m_new && m==n_new))//make sure the edge is not the same as the one just removed
        {
        here:
            n_new = rand_btw(1,num_nodes,0);//select two random nodes
            m_new = rand_btw(1,num_nodes,n_new);
            while(branchN[n_new]==0)//make sure random node is in the correct branch
                n_new = rand_btw(1,num_nodes,0);
            while(branchM[m_new]==0)
                m_new = rand_btw(1,num_nodes,0);
        }
        
        while(next_population[j].gene[n_new][m_new].weight ==MAX_ENERGY)//while edge is not possible, find another edge until a possible edge is found
        {
            n_new = rand_btw(1,num_nodes,0);//select two random nodes
            m_new = rand_btw(1,num_nodes,n_new);
            
            while(branchN[n_new]==0)//make sure random node is in the correct branch
                n_new = rand_btw(1,num_nodes,0);
            while(branchM[m_new]==0)
                m_new = rand_btw(1,num_nodes,0);
            while((n==n_new && m==m_new)|| (n==m_new && m==n_new))//make sure the edge is not the same as the one just removed
            {
                goto here;
            }
        }
        
        if (source ==0)
        {
            printf("\n\tAdd Edge (%d %d)",n_new,m_new);
            next_population[j].gene[n_new][m_new].label= 1;//add this random edge to the solution
        }
        else
        {
            printf("\n\tAdd Edge (%d %d)",m_new,n_new);
            next_population[j].gene[m_new][n_new].label= 1;
        }
        edgeAdded = TRUE;
    }
    //fix order of branches
    dfs_swap( 1, num_nodes, next_population,j, visited);
    return 0;
}

void *dfs_swap(int v, int num_nodes,chrom* next_population,int x ,int visited[])
{
    int i;
    
    visited[v]=1;//mark the node as visited
    
    for (i=1;i<=num_nodes;i++)//for all edges
    {
        //  if (i!=x &&v !=y)//not including newly added edge
        if(!visited[i]==1)//with nodes not yet visited
        {
            if(next_population[x].gene[v][i].label !=1)//swap if wrong order
                if(next_population[x].gene[i][v].label ==1)//check is possible
                {
                    next_population[x].gene[v][i].label = 1;
                    next_population[x].gene[i][v].label = 0;
                    printf("\n switch1 order of edge (%d %d) to (%d %d)",i,v,v,i);
                }
            if(next_population[x].gene[v][i].label ==1)
            {
                dfs_swap(i, num_nodes, next_population,x,visited);
            }
        }
    }
    return 0;
}

//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************
//***********************************************************************************************************************

void *selection_tournament(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)
{
    int i,x,y;
    int rand1_individual,rand2_individual;
    int rand_r, tournament_size= 8;
    chrom* copy;
    
    //allocate memory of copy
    copy = malloc(num_nodes * sizeof(copy));
    if(copy == NULL)
        fprintf(fp,"ERROR11: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        copy[x].gene = malloc(num_nodes * sizeof(edge*));
        if(copy[x].gene == NULL)
            fprintf(fp,"ERROR12: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            copy[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(copy[x].gene[y] == NULL)
                fprintf(fp,"ERROR13: Memory allocation failed!");
        }
    }
    
    for (i=1; i<pop_size+1; i++)//copy over original data
        copy[i] = next_population[i];
    
    printf("\n\n\tTournament selection:");
    
    for(i=1; i<pop_size+1; i++)
    {
        rand1_individual = rand_btw(1,pop_size,0);//get random individuals
        rand2_individual = rand_btw(1,pop_size,rand1_individual);//get random individuals
        
        rand_r = generate_rand(0,10);//get random number r
        printf("\nrand1:%d(%.1f), rand2:%d(%.1f), randr:%d, toursize:%d",rand1_individual,next_population[rand1_individual].fitness_score,rand2_individual, next_population[rand2_individual].fitness_score,rand_r,tournament_size);
        
        if(rand_r<tournament_size) // if r is less than the predetermined tournament size thatn select the fittest individula
        {
            if(next_population[rand1_individual].fitness_score < next_population[rand2_individual].fitness_score) // store the fittest individual
                copy[i]=next_population[rand1_individual];
            else
                copy[i]=next_population[rand2_individual];
        }
        else
        {
            if(next_population[rand1_individual].fitness_score > next_population[rand2_individual].fitness_score) // store the fittest individual
                copy[i]=next_population[rand1_individual];
            else
                copy[i]=next_population[rand2_individual];
        }
    }
    
    printf("\n\n\tNew population:");
    for(i=1; i<pop_size+1; i++)//print out
    {
        next_population[i] = copy[i];
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    fflush(NULL);
    return(0);
}

void *selection_roulettewheel(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)
{
    int i,j,x,y;
    float r_random;
    float likelihood[pop_size],sum_likelihood[pop_size+1];
    float total_fitness=0.0;
    chrom* store_memory;//store the data of original values
    
    //allocate memory of store_memory
    store_memory = malloc(num_nodes * sizeof(store_memory));
    if(store_memory == NULL)
        fprintf(fp,"ERROR14: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        store_memory[x].gene = malloc(num_nodes * sizeof(edge*));
        if(store_memory[x].gene == NULL)
            fprintf(fp,"ERROR15: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            store_memory[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(store_memory[x].gene[y] == NULL)
                fprintf(fp,"ERROR16: Memory allocation failed!");
        }
    }
    for (i=1; i<pop_size+1; i++)//copy over original data
        store_memory[i] = next_population[i];
    
    sum_likelihood[0]=0.0;
    printf("\n\n\tRoulette wheel selection:");
    
    for(i=1; i<pop_size+1; i++) // calculate total fitness of population
        total_fitness +=  next_population[i].fitness_score;//calculate total fitness of the population
    
    for(j=1; j<pop_size+1; j++) // for each chromosome
    {
        likelihood[j] = (next_population[j].fitness_score/total_fitness); // calculate the prob of each chromosome
        sum_likelihood[j] = sum_likelihood[j-1] + likelihood[j];// calculate the total prob of each chromosome
        printf("\n\tlikelihood %.2f",likelihood[j]);
        printf("\tsum of likelihood %.2f",sum_likelihood[j]);
    }
    
    for(j=1; j<pop_size+1; j++)
        for(i=1; i<pop_size+1; i++)
        {
            printf("\n\trandom %.2f and sumlikeli[%d] %.2f",r_random,i,sum_likelihood[i]);
            r_random =generate_rand_float();
            if ((r_random> sum_likelihood[i]) && (likelihood[i]<likelihood[i+1]))
                store_memory[i]=next_population[i];
        }
    
    printf("\n\n\tNew population:");
    for(i=1; i<pop_size+1; i++) //print out new chromosome
    {
        next_population[i] = store_memory[i];
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    fflush(NULL);
    return(0);
}

void *selection_rank(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)
{
    int i,k;
    int x,y;
    float likelihood;
    float total=0, rand_number;
    chrom* store_data;//store the data of original values
    
    //allocate memory of store_data
    store_data = malloc(num_nodes * sizeof(chrom));
    if(store_data == NULL)
        fprintf(fp,"ERROR17: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        store_data[x].gene = malloc(num_nodes * sizeof(edge*));
        if(store_data[x].gene == NULL)
            fprintf(fp,"ERROR18: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            store_data[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(store_data[x].gene[y] == NULL)
                fprintf(fp,"ERROR19: Memory allocation failed!");
        }
    }
    
    printf("\n\n\tRank selection:");
    selection_elitism_sort(num_nodes, pop_size, current_generation,next_population, fp,1);
    
    for (i=1; i<pop_size+1; i++)//copy over original data
        store_data[i] = next_population[i];
    
    for(i=1; i<pop_size+1; i++) // create new population
    {
        k=0;
        rand_number = generate_rand(0,100);// get random number
        likelihood = 100.0/(pop_size); // generate probability of each chromosomes occuring
        do
        {
            k++;
            total = total + likelihood*k;
        }
        while (total < rand_number);
        store_data[i]=next_population[k];
    }
    
    printf("\n\n\tNew population:");
    for(i=1; i<pop_size+1; i++) //print out new chromosome
    {
        next_population[i] = store_data[i];//put data back
        print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    }
    fflush(NULL);
    return(0);
}

void *selection_elitism_sort(int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp, int show)
{
    //descending order
    // population is sorted according to their fitness such that there is no way that strong and weak chromosomes reproduce.
    int i,j;
    int x,y;
    chrom* temporary;
    
    //allocate memory of temporary
    temporary = malloc(num_nodes * sizeof(chrom));
    if(temporary == NULL)
        fprintf(fp,"ERROR20: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        temporary[x].gene = malloc(num_nodes * sizeof(edge*));
        if(temporary[x].gene == NULL)
            fprintf(fp,"ERROR21: Memory allocation failed!");
        for(y = 1; y <= num_nodes; y++)
        {
            temporary[x].gene[y] = malloc(num_nodes * sizeof(edge));
            if(temporary[x].gene[y] == NULL)
                fprintf(fp,"ERROR22: Memory allocation failed!");
        }
    }
    if(show ==1)
        printf("\n\n\tElitism (sorting) selection:");
    
    for(i=1; i<=pop_size; i++)
        for(j=1; j<pop_size; j++) //dont include last chromosome as it has nothing to compare with
            if(next_population[j+1].fitness_score > next_population[j].fitness_score) // if the next chromosome is better than the previous (maximizing)
            {
                temporary[1] = next_population[j+1]; // store the chromosome
                next_population[j+1] = next_population[j]; // then switch them around such that chromosomes are ordered from best to worst
                next_population[j] = temporary[1];
            }
    
    //printf("\n\n\tDescending order of fitness:");
    for(i=1; i<=pop_size; i++)
        if (show==1)
            print_out_chroms(num_nodes,pop_size,next_population,i,0,current_generation,fp);
    
    fflush(NULL);
    return(0);
}

//***********************************************************************************************************************
void print_out_chroms(int num_nodes,int pop_size,chrom* next_population, int j, int best, int current_generation, FILE *fp)
{
    int x,y;
    if (best == TRUE)
    {
        printf("\n\n\tBest chromosome of Generation %d:\n \tChromosome[%d] =", current_generation, j);
        for(x=1; x<=num_nodes; x++)
        {
            fprintf(fp,".");
            for(y=1; y<=num_nodes; y++)
            {
                fprintf(fp,"%d",next_population[j].gene[x][y].label);
                if(next_population[j].gene[x][y].label==1)
                    printf("(%d %d):%.1f  ",x,y,next_population[j].gene[x][y].weight);
            }
        }
        next_population[j].fitness_score = Objective_function(num_nodes,pop_size,j,next_population); //calculate fitness score of chromosome
        printf(" Fitness=%.2f",next_population[j].fitness_score); // print the fitness and value of chromosome for each respective chromosome
    }
    else
    {
        printf("\n\tChromosome[%d] = ",j);
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)
                if(next_population[j].gene[x][y].label==1)
                    printf("(%d %d):%.1f  ",x,y,next_population[j].gene[x][y].weight);
        
        next_population[j].fitness_score = Objective_function(num_nodes,pop_size,j,next_population); //calculate fitness score of chromosome
        printf(" Fitness=%.2f",next_population[j].fitness_score); // print the fitness and value of chromosome for each respective chromosome
    }
}

void gen_init_pop(int num_nodes, int pop_size, int visited[], chrom *temp, chrom *current_population)
{
    int num_edges=1;
    int rand;
    int i,j, x,y,n;
    printf("\n\n\t Initial population");
    //make random solutions for initial populations
    for(i=1; i<=pop_size; i++) //for each chromosome
    {
        for(j=1; j<=num_nodes; j++) //for each solution clear the visited list
            visited[j]=0;
        
        num_edges =1;
        printf("\n\n\tChomosome: %d ",i);
        visited[1]=1;//set node one as the source node
        
        while(num_edges < num_nodes)//have required #edges = #node-1
        {
            for(x=1; x<=num_nodes; x++)
                for(y=1; y<=num_nodes; y++)//for all edges
                    if(visited[x]!=0)//if node is visited
                        if(current_population[i].gene[x][y].weight!=MAX_ENERGY) //if edge is possible
                            if(visited[y]==0)//if node is not yet visited
                            {
                                rand = rand_binary();
                                current_population[i].gene[x][y].label=rand;//randomly add an edge
                                if (rand == 1)
                                {
                                    visited [y] = 1;
                                    printf("\n\t Edge1 %d:(%d %d) energy:%.1f",num_edges++,x,y,current_population[i].gene[x][y].weight);
                                    for(n=1; n<=num_nodes; n++)
                                        if(n!=y)//find different edge connected to node x
                                            if (current_population[i].gene[x][n].weight != MAX_ENERGY)//if edge is possible
                                                if(current_population[i].gene[x][n].weight<= current_population[i].gene[x][y].weight)// node can reach all other nodes with equal or lower tx power
                                                    if(visited[n]==0)//if node is not yet visited
                                                    {
                                                        visited[n]=1;
                                                        current_population[i].gene[x][n].label = 1;
                                                        printf("\n\t Edge2 %d:(%d %d) energy:%.1f",num_edges++,x,n,current_population[i].gene[x][n].weight);
                                                        current_population[i].gene[x][n].weight=current_population[i].gene[n][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                                                    }
                                    current_population[i].gene[x][y].weight=current_population[i].gene[y][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                                }
                            }
        }
        
    }
    //transfer original weight back
    for(i=1; i<=pop_size; i++)
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)
                current_population[i].gene[x][y].weight = temp[i].gene[x][y].weight;
    
}

void get_network_topology(int num_nodes, int pop_size, int visited[], chrom *temp, chrom *current_population)
{
    int i,x,y;
    float distance;
    
    //ask for the energy needed of each edge the node takes and store them
    printf("\n\tEnter the network topology matrix:\n\t");
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++)
        {
            scanf("\t%f", &distance);
            for(i=1; i<=pop_size; i++)
                current_population[i].gene[x][y].weight = distance;
        }
    for(i=1; i<=pop_size; i++)
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)
            {
                temp[i].gene[x][y].weight = current_population[i].gene[x][y].weight;//store data of weights as initial_SA.weight will be modified temporarily for calculations
                current_population[i].gene[x][y].label =0;//initially start with no tree
                visited[x]=0;//no nodes visited at the start
            }
}

float Objective_function(int num_nodes,int pop_size,int i,chrom *next_population)
{
    
    float new_cost=0.0, tree_cost = 0.0, fitness_score = 0.0;
    float max_cost =0.0;
    int j;
    
    //calculate the highest cost for each population ---> make sure this function is integrated correctly to the main function
    for(j=1;j<=pop_size;j++)
    {
        new_cost = cost(num_nodes,j,next_population);
        //printf("\nPOP[%d]cost:%.1f  ",j,new_cost);
        if (max_cost< new_cost)
            max_cost = new_cost;//max_cost is the worst solution in the population
    }
    printf("\n\tMaxCost = %.1f ",max_cost);
    
    
    //calculate the cost of solution
    tree_cost = cost(num_nodes,i,next_population);
    printf("ChromCost = %.1f ",tree_cost);
    //fitness_score = max_cost - tree_cost; //at worst, the fitness function is 0.
    fitness_score = tree_cost;
    //fitness function is relative to the population
    //this minimises the function i.e. reverses the cost
    
    return(fitness_score);
}

float cost(int num_nodes,int i,chrom *next_population)
{
    float new_cost = 0.0;
    int x,y;
    float weight;
    
    for(x=1; x<=num_nodes; x++)//for all edges
    {
        weight =0.0;//find the largest tx on each node
        for(y=1; y<=num_nodes; y++)
            if(next_population[i].gene[x][y].label == 1)
                if(weight<next_population[i].gene[x][y].weight)//find highest energy used per node
                    weight = next_population[i].gene[x][y].weight;
        
        new_cost += weight;//find total weight
    }
    //printf("\n\t Total cost is %.1f", new_cost);
    
    return new_cost;
}

void *do_mutation_operator(int mutation,int num_nodes,int pop_size,float mutation_rate, chrom* next_population,FILE *fp)
{
    switch (mutation)
    {
        case 1 :
            mutation_addRemove(num_nodes, pop_size, mutation_rate, next_population,fp);
            break;
        case 2 :
            //mutation_loopRemove(num_nodes, pop_size, next_population,fp);
            break;
        default :
            printf("invalid input1\n" );
            break;
    }
    return 0;
}

void *do_crossover_operator(int crossover,int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)
{
    switch (crossover)
    {
        case 1 :
            crossover_sameBranches( num_nodes, pop_size,current_generation, next_population, fp);
            break;
        case 2 :
            crossover_swapBranches( num_nodes, pop_size,current_generation, next_population,fp);
            break;
        default :
            printf("invalid input2\n" );
            break;
    }
    return 0;
}

void *do_selection_operator(int selection,int num_nodes,int pop_size,int current_generation, chrom* next_population,FILE *fp)
{
    switch (selection)
    {
        case 1 :
            selection_elitism_sort(num_nodes, pop_size, current_generation, next_population,fp,1);
            break;
        case 2 :
            selection_tournament(num_nodes, pop_size, current_generation, next_population,fp);
            break;
        case 3 :
            selection_rank(num_nodes, pop_size, current_generation, next_population,fp);
            break;
        case 4 :
            selection_roulettewheel(num_nodes, pop_size, current_generation, next_population,fp);
            break;
        default :
            printf("invalid input3\n" );
            break;
    }
    return 0;
}

int generate_rand(int Min, int Max)
{
    // Generates random number between min and max, inclusive.
    //From: http://cboard.cprogramming.com/c-programming/145187-how-pick-random-number-between-x-y.html
    
    int range, result, cutoff;
    
    if (Min >= Max)
        return Min; // only one outcome possible, or invalid parameters
    range = Max-Min+1;
    cutoff = (RAND_MAX / range) * range;
    
    // Rejection method, to be statistically unbiased.
    do
    {
        result = rand();
    }
    while (result >= cutoff);
    
    return result % range + Min;
}

int rand_btw(int Min, int Max,int not_including)
{
    //     Generates random number between min and max, inclusive.
    //    From: http://cboard.cprogramming.com/c-programming/145187-how-pick-random-number-between-x-y.html
    
    int range, result, cutoff,random;
redo:
    if (Min >= Max)
        return Min; // only one outcome possible, or invalid parameters
    range = Max-Min+1;
    cutoff = (RAND_MAX / range) * range;
    
    //     Rejection method, to be statistically unbiased.
    do
    {
        result = rand();
    }
    while (result >= cutoff);
    random = result % range + Min;
    if(not_including !=0 )
        if(random==not_including)
            goto redo;
    return random;
}

int rand_binary(void)
{
    int random;
    
    random = rand()%2;
    return random;
}

float generate_rand_float(void)
{
    float random_value; // random value betw
    
    random_value = ((float)rand()/(float)(RAND_MAX+1));//version 1
    //Alternatively:
    //random_value = ((rand()%10000000)/10000000);
    
    return random_value;
}

int get_restart(void)
{
    int restart; //number of chromosomes entered
    
    printf("\n\n\n\n\tWould you like to restart? (1 = yes)\t");// ask if they want to restart
    scanf("%d",&restart);
    
    return restart;
}

float get_mutation_rate(void)
{
    float mutation_rate; //number of chromosomes entered
    
enter:
    printf("\tPlease enter (float) mutation rate (Minimum %d to Maximum %d):\t",MIN_MUTATION_RATE,MAX_MUTATION_RATE);// Ask for number of chromosomes
    scanf("%f",&mutation_rate); //get the number of chromosomes
    
    if(mutation_rate <MIN_MUTATION_RATE || mutation_rate>MAX_MUTATION_RATE)
        goto enter;
    
    return mutation_rate;
}

int get_chromosome_size(void)
{
    int num_nodes; //problem size
    
enter:
    printf("\tPlease enter no. of Nodes considered (Min %d to Max %d):\t",MIN_CHROMO_SIZE,MAX_CHROMO_SIZE);// Ask for number of chromosomes
    scanf("%d",&num_nodes); //get the number of chromosomes
    
    
    if(num_nodes <MIN_CHROMO_SIZE || num_nodes>MAX_CHROMO_SIZE)
        goto enter;
    return (num_nodes);
}

int get_population_size(void)
{
    int pop_size; //number of population entered
    //
    //    enter:
    //    printf("\tPlease enter an even population size (Minimum %d to Maximum of %d):\t",MIN_POP_SIZE,MAX_POP_SIZE);// Ask for number of population
    //    scanf("%d",&pop_size); //get the number of population
    //
    //    if(pop_size <MIN_POP_SIZE || pop_size>MAX_POP_SIZE)
    //        goto enter;
    //    else if(pop_size%2 !=0)
    //        goto enter;
    
    return pop_size=4;
}

int get_mutation_strategy(void)
{
    int mutation; //number of population entered
    
enter:
    printf("\n\tChoose a mutation strategy: \
           \n\t\tAdd/Remove        \t 1       \
           \n\t\tLoop/Remove       \t 2\
           \n\t\tYou picked:");// Ask for mutation strategy
    
    scanf("%d",&mutation); //get the number of population
    
    if(mutation <1 || mutation>2)
        goto enter;
    return mutation;
}

int get_crossover_strategy(void)
{
    int crossover; //number of population entered
    
enter:
    printf("\n\tChoose a crossover strategy: \
           \n\t\tSame Branches    \t 1       \
           \n\t\tSwap Branches    \t 2       \
           \n\t\tYou picked:");// Ask for crossover strategy
    
    scanf("%d",&crossover); //get the number of population
    
    if(crossover <1 || crossover>2)
        goto enter;
    return crossover;
}

int get_selection_strategy(void)
{
    int selection; //number of population entered
    
enter:
    printf("\n\tChoose a selection strategy: \
           \n\t\tElitism         \t 1       \
           \n\t\tTournament      \t 2       \
           \n\t\tRank            \t 3       \
           \n\t\tRoulette Wheel  \t 4       \
           \n\t\tYou picked:");// Ask for selection strategy
    
    scanf("%d",&selection); //get the number of population
    
    if(selection <1 || selection>4)
        goto enter;
    return selection;
}

int get_num_generation(void)
{
    int num; //number of generations entered
    
enter:
    printf("\tPlease enter the number of generations (Minimum 1 to Maximum of %d):\t",MAX_GENERATIONS);// Ask for number of generation for stopping criterion
    scanf("%d",&num); //get the number of generations
    
    if(num < 1 || num > MAX_GENERATIONS)
        goto enter;
    return num;
}

void introduction (void)
{
    printf("\n\n\n\n\n\n\tWelcome to the Genetic Algorithm coded by Daniella Bacud\n"); // Print introduction
}