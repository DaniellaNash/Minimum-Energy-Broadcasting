//
//  Simulated Annealing.c
//  MEB
//
//  Created by Daniella Bacud on 27/07/2016.
//  Copyright © 2016 Daniella Bacud. All rights reserved.
//

#include "Simulated Annealing.h"

void *dfs(int v, int num_nodes, edge** new_solution ,int visited[])
{
    int i;
    
    visited[v]=1;
    for (i=1; i<=num_nodes; i++)
        if(new_solution[v][i].label==1||new_solution[i][v].label==1)
            if (!visited[i]==1) //if not yet visited
                dfs(i, num_nodes, new_solution,visited);
    return 0;
}

void *dfs_swap(int v, int num_nodes, edge** new_solution ,int visited[])
{
    int i;
    
    visited[v]=1;//mark the node as visited
    
    for (i=1;i<=num_nodes;i++)//for all edges
    {
        //  if (i!=x &&v !=y)//not including newly added edge
        if(!visited[i]==1)//with nodes not yet visited
        {
            if(new_solution[v][i].label !=1)//swap if wrong order
                if(new_solution[i][v].label ==1)//check is possible
                {
                    new_solution[v][i].label = 1;
                    new_solution[i][v].label = 0;
                    printf("\n switch1 order of edge (%d %d) to (%d %d)",i,v,v,i);
                }
            if(new_solution[v][i].label ==1)
            {
                dfs_swap(i, num_nodes, new_solution,visited);
            }
            
        }
    }
    return 0;
}

int rand_binary(void)
{
    int random;
    
    random = rand()%2;
    
    return random;
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

float rand_float(void)
{
    float random;
    random = ((float) rand()) / (float) RAND_MAX;
    return random;
}

float cost(int num_nodes, edge** new_solution )
{
    float new_cost = 0.0;
    int num_edges = 1;//set number of edges back to default
    int x,y;
    float weight;
    
    for(x=1; x<=num_nodes; x++)//print out new_solution
    {
        weight =0.0;//find the largest tx on each node
        for(y=1; y<=num_nodes; y++)
            if(new_solution[x][y].label == 1)
            {
                printf("\n Edge %d:(%d %d) energy:%f",num_edges++,x,y,new_solution[x][y].weight);
                if(weight<new_solution[x][y].weight)//find highest energy used per node
                {
                    weight = new_solution[x][y].weight;
                    //printf("\n weigth:%d accum:%d",weight, new_cost);
                }
            }
        new_cost += weight;//find total weight
    }
    printf("\n Total cost is %f\n\n", new_cost);
    return new_cost;
}

float acceptance_prob(float T, float old_cost, float new_cost,edge** new_solution , edge** current_SA ,FILE *fp, int num_nodes )
{
    int x,y;
    float ap = 0.0, delta=0.0;
    float cost;
    float random;
    
    delta = (old_cost-new_cost);
    ap = (exp(delta/T)); //this is the typical equation used
    
    if(delta >0)//if new_solution has less energy select it...i.e. if new_cost<old_cost
    {
        fprintf(fp,"NO");
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)
                current_SA[x][y] = new_solution[x][y];
        old_cost = new_cost;
    }
    else //if new_solution uses more energy maybe select it
    {
        random = rand_float();//random number between 0 and 1
        
        if (ap > random)//if ap is closer to one the accpet
        {
            for(x=1; x<=num_nodes; x++)
                for(y=1; y<=num_nodes; y++)
                    current_SA[x][y] = new_solution[x][y];
            old_cost = new_cost;
        }
    }
    cost = old_cost;
    
    return cost;
}

void *add_edge(int num_nodes,edge** new_solution , int n, int m ,FILE *fp)
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
        dfs(n,num_nodes,new_solution,branchN);
        dfs(m,num_nodes,new_solution,branchM); //find nodes in n branch using DFS
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
        
        while(new_solution[n_new][m_new].weight ==MAX_ENERGY)//while edge is not possible, find another edge until a possible edge is found
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
            printf("\n Add Edge1 (%d %d)",n_new,m_new);
            new_solution[n_new][m_new].label= 1;//add this random edge to the solution
        }
        else
        {
            printf("\n Add Edge2 (%d %d)",m_new,n_new);
            new_solution[m_new][n_new].label= 1;
        }
        edgeAdded = TRUE;
    }
    //fix order of branches
    dfs_swap( 1, num_nodes, new_solution, visited);
    return 0;
}

void *neighbour_solution(int num_nodes,edge** new_solution ,FILE *fp)
{
    int set;
    int n,m;
    int edgeRemoved =FALSE;
    int deadend =FALSE;
    
    //remove a random edge
    while (edgeRemoved == FALSE)//remove a random edge
    {
    redo:
        n = rand_btw(1,num_nodes,0);//select two random nodes to remove the edge from
        m = rand_btw(1,num_nodes,n);//node [0]=n, m =m
        
        while(new_solution[n][m].label ==0)//make sure that the edge is in the current solution.//while edge is not in solution, find another edge until solution edge is found. Note, if edge is in current solution then edge is also possible i.e. has a weight>0
        {
            n = rand_btw(1,num_nodes,0);
            m = rand_btw(1,num_nodes,n);//prevent picking the same node
        }
        
        //temporarily delete edge
        set = temp_delete( new_solution, n, m);
        
        deadend = check_deadend(num_nodes,new_solution,n,m,fp);
        
        while(deadend ==TRUE)//while delete branch is a deadend
        {
            if(set==0)
                new_solution[m][n].label =1;
            else
                new_solution[n][m].label =1;   //undelete the branch
            goto redo;
        }
        printf("\n Remove Edge (%d %d)",n,m);
        new_solution[n][m].label =new_solution[m][n].label= 0;//remove this random edge from the solution
        edgeRemoved = TRUE;//this separates the tree into two branches
    }
    
    //connect the two branches back together to make the solution tree
    add_edge( num_nodes, new_solution,n, m,fp);
    return 0;
}

int check_deadend(int num_nodes,edge** new_solution ,int n,int m ,FILE *fp)
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
    
    dfs(n,num_nodes,new_solution,branchN);
    dfs(m,num_nodes,new_solution,branchM); //find nodes in n branch using DFS
    //there has to be more than one possible edge for branch
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++) //for each edge from each branch
        {
            if(branchN[x]==1&&branchM[y]==1)//to the other branch
                if(new_solution[x][y].weight !=MAX_ENERGY)//that is possible
                    count1 ++;
            if (branchM[x]==1&&branchN[y]==1)
                if(new_solution[x][y].weight !=MAX_ENERGY)//that is possible
                    count2 ++;
        }
    if (count1<2||count2<2)
        deadend = TRUE;
    return deadend;
}

int temp_delete(edge** new_solution ,int n,int m)
{
    int set;
    if (new_solution[m][n].label ==1)
    {
        set = 0;
        new_solution[m][n].label= 0;
    }
    if (new_solution[n][m].label ==1)
    {
        set=1;
        new_solution[n][m].label= 0;
    }
    return set;
    
}

void *print_output_file(int iteration, float old_cost, int num_nodes,edge** current_SA , FILE *fp)
{
    int x,y;
    
    fprintf(fp,"\n\t\t%i\t\t\t%f\t\t",iteration,old_cost );
    for(x=1; x<=num_nodes; x++)//print out new_solution
        for(y=1; y<=num_nodes; y++)
            if(current_SA[x][y].label == 1)
                fprintf(fp,"(%d -> %d) ",x,y);
    return 0;
}

void *gen_init_solution(int num_edges, int num_nodes, int visited[],edge** initial_SA ,edge** temp ,edge** current_SA )
{
    int x,y,rand,n;
    
    while(num_edges < num_nodes)//have required #edges = #node-1
    {
        for(x=1; x<=num_nodes; x++)
            for(y=1; y<=num_nodes; y++)//for all edges
                if(visited[x]!=0)//if node is visited
                    if(initial_SA[x][y].weight!=MAX_ENERGY) //if edge is possible
                        if(visited[y]==0)//if node is not yet visited
                        {
                            rand = rand_binary();
                            initial_SA[x][y].label=rand;//randomly add an edge
                            if (rand == 1)
                            {
                                visited [y] = 1;
                                printf("\n Edge1 %d:(%d %d) energy:%f, label:%d",num_edges++,x,y,initial_SA[x][y].weight,initial_SA[x][y].label);
                                for(n=1; n<=num_nodes; n++)
                                    if(n!=y)//find different edge connected to node x
                                        if (initial_SA[x][n].weight != MAX_ENERGY)//if edge is possible
                                            if(initial_SA[x][n].weight<= initial_SA[x][y].weight)// node can reach all other nodes with equal or lower tx power
                                                if(visited[n]==0)//if node is not yet visited
                                                {
                                                    visited[n]=1;
                                                    initial_SA[x][n].label = 1;
                                                    printf("\n Edge2 %d:(%d %d) energy:%f, label:%d",num_edges++,x,n,initial_SA[x][n].weight,initial_SA[x][n].label);
                                                    initial_SA[x][n].weight=initial_SA[n][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                                                }
                                initial_SA[x][y].weight=initial_SA[y][x].weight=MAX_ENERGY;//make sure same edge is not chosen twice
                            }
                        }
    }
    
    //transfer original weight back
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++)
            initial_SA[x][y].weight = temp[x][y].weight;
    
    //set current solution to the initial solution generated and set it as our best solution so far
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++)
            current_SA[x][y] = initial_SA[x][y];
    return 0;
}

void *get_network_topology( int num_nodes, edge **initial_SA, edge** temp , int visited[])
{
    int x,y;
    float distance;
    //ask for the energy needed of each edge the node takes and store them
    printf("\nEnter the network topology matrix:\n");
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++)
        {
            scanf("%f",&distance);//Note that 0 = out of tx range of the node
            if (distance !=0&& distance < MAX_DISTANCE)
                initial_SA[x][y].weight = pow(distance,2);
            else
                initial_SA[x][y].weight = MAX_ENERGY;
            
        }
    
    for(x=1; x<=num_nodes; x++)
        for(y=1; y<=num_nodes; y++)
        {
            temp[x][y].weight = initial_SA[x][y].weight;//store data of weights as initial_SA.weight will be modified temporarily for calculations
            initial_SA[x][y].label =0;//initially start with no tree
            visited[x]=0;//no nodes visited at the start
        }
    
    
    visited[1]=1;//set node one as the source node
    return 0;
    
}

int get_num_nodes(void)
{
    int num_nodes;
    
    printf("\nEnter the number of nodes:");
    scanf("%d",&num_nodes);
    
    return num_nodes;
}

int main()
{
    //initialise data
    float T = 1.0;//"Usually, the temperature is started at 1.0 and is decreased at the end of each iteration by multiplying it by a constant called α" 7
    //by http://katrinaeg.com/simulated-annealing.html
    //typical choices are between 0.8 and 0.99.
    float T_min = 0.00001; //min temp to be reached
    float alpha = 0.9;//depreciation constant used for temperature after each iteration
    float old_cost=0,new_cost=0;//set cost default setting
    int compare_process=1; //set neighbour solution default setting
    //declare and initialise other variables
    int x,y;
    int iteration =1;
    int *visited;
    int num_nodes =0,num_edges=1;
    
    FILE *fp;
    
    //creating new file with write mode
    fp=fopen("testSA.txt","w");
    if(fp==NULL)
    {
        printf("Error!");
        exit(1);
    }
    fprintf(fp,"%s %s\t%s %s\t%s\n" ,"Iteration","number","Total",
            "Energy","Solution");
    
    srand(time(NULL));// refresh random values at every execution
    
    num_nodes = get_num_nodes();
    edge** initial_SA;
    edge** current_SA;
    edge** new_solution;
    edge** temp;
    
    initial_SA = malloc(num_nodes * sizeof(edge*));
    current_SA = malloc(num_nodes * sizeof(edge*));
    new_solution = malloc(num_nodes * sizeof(edge*));
    temp = malloc(num_nodes * sizeof(edge*));
    visited = (int *)malloc(num_nodes * sizeof(int));
    if(initial_SA == NULL||current_SA == NULL||new_solution == NULL||temp == NULL||visited==NULL)
        fprintf(fp,"ERROR: Memory allocation failed!");
    for(x = 1; x <= num_nodes; x++)
    {
        initial_SA[x] = malloc(num_nodes * sizeof(edge));
        current_SA[x] = malloc(num_nodes * sizeof(edge));
        new_solution[x] = malloc(num_nodes * sizeof(edge));
        temp[x] = malloc(num_nodes * sizeof(edge));
        
        if(initial_SA[x] == NULL||current_SA[x] == NULL||new_solution[x] == NULL||temp[x] == NULL)
            fprintf(fp,"ERROR: Memory allocation failed!");
    }
    
    get_network_topology(num_nodes,initial_SA,temp,visited);//get network
    //generate_rand_matrix( num_nodes,initial_SA,temp , visited);
    gen_init_solution(num_edges, num_nodes, visited,initial_SA, temp, current_SA);//generate initial random solution (TREE MST - no loops) good initial solution to start with
    old_cost = cost( num_nodes, current_SA);//print solution and cost
    
    printf("\n First iteration cost = %f\n",old_cost);
    
    T = old_cost*1.5;//make To relative to initial cost by a factor of 1 to 2
    
    //main algorithm
    while (T > T_min)//max generation //CHANGE THIS BACK TO T<=T_min
    {
        print_output_file(iteration, old_cost, num_nodes, current_SA, fp);
        printf("\n\n    Iteration: %d",iteration++);
        compare_process=1;
        
        while (compare_process <= 100)//"neighbor-cost-compare-move process
            //process is carried about many times (typically somewhere between 100 and 1,000) at each temperature"by http:katrinaeg.com/simulated-annealing.html
        {
            printf("\n\n Neighbor-cost-compare-move iteration: %d",compare_process);
            //neighbour function
            for(x=1; x<=num_nodes; x++)
                for(y=1; y<=num_nodes; y++)
                    new_solution[x][y] = current_SA[x][y];
            
            neighbour_solution( num_nodes, new_solution,fp);
            
            new_cost = cost(num_nodes, new_solution);//print out solution and total cost of new solution
            //acceptance probability function
            old_cost = acceptance_prob(T, old_cost, new_cost, new_solution, current_SA, fp, num_nodes);
            compare_process += 1;
        }
        //temperature decreases by a factor of alpha after each generation until the minimum temp is reached
        T = T*alpha;
    }
    
    old_cost = cost( num_nodes, current_SA);//print solution and cost
    return 0;
}