#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

/*
Dijkstra Resolver for Builder Tier 2 - Computer Architecture II
Version 10


@Author: Luis Rodríguez, Alberto Merino, Álvaro Molinero
*/


/*
Type Neighbor represents the information of a neighbor,
which is composed of the name of the neighbor and the cost to it
*/
typedef struct {
	char neighbor; //name of the neighbor
	int cost; //the cost to it from the node that has it as a neighbor
} Neighbor;

/*
Type Node represents each node, which has information about its neighbors.
This is only used when reading the input.
*/
typedef struct {
	char name; //name of this node
	Neighbor* neighbor; //array with all its neighbors
	int n_neighbors;//number of neighbors
} Node;

/*
Type Destination represents each destination that the computing_node has.
*/
typedef struct {
	char name; //name of this destination node
	char previous_in_path;//p(x)
	int cost; //D(x) the cost to it, computed using Dijkstra until this point- overflows at 32767 (assuming 2 bytes), which will be our infinite value
	char minimum_cost; //flag that indicates if the current cost computed is the minimum or not - (1->yes,0->no) //is char only because it ocupies only 1 byte
	Neighbor* neighbor; //array with all the neighbors of this destination
	int n_neighbors;//number of neighbors of this destination
} Destination;

/*
Type Main_Node represents the computing node.
*/
typedef struct {
	char name; //name of the computing node
	Destination* node; //array with all its destinations
} Main_Node;

void Welcome();//explains program
int GetNumberFromString(char *argv);//*Implemented Get a number that the string represents
int Min(int a, int b);//*Implemented Computes the minimum of two numbers
int Read_Number(int lower_limit, int upper_limit);//*Implemented reads a number in a safe way from user
char Read_Char();//*Implemented reads a char in a safe way from user
int Read_Number_Nodes();//*Implemented just reads number of nodes in network from user
void Print_Header_Dijksra_Table(Destination* node, int number_of_nodes);//*Implemented prints the names of the columns of the table
Node* Read_input(int number_of_nodes);//*Implemented Reads input from user and puts it in an array of Nodes
Main_Node Create_Computing_Node(Node* input, int number_of_nodes);//*Implemented initializes Main_Node from the input array
void Print_step(Main_Node computing_node, int number_of_nodes, int step, char* n_prime);//*Implemented prints current state of computation
Destination Search_neighbor_min_d_cost(Main_Node* computing_node, int number_of_nodes, char* n_prime);//*Implemented looks for who is the current node now, the one with minimum cost
void Recompute_cost_to_one_destination(Destination* destination, Destination current_node);//*Implemented almost the most important function (and shortest...)- updates cost to a destination
void Free(Main_Node computing_node, int number_of_nodes);//*Implemented frees the memory of all the program
void Check_memory_allocation(void*);//*Implemented Called whenever dynamic memory has being allocated

int main()
{
	//Variables
	Main_Node computing_node;
	Node* input;//result of input from user
	int step = 0;//current step in Dijkstra algorithm
	Destination current_node;//current node that we are working with in Dijkstra algorithm
	int number_of_nodes=0;
	int i;
	char* n_prime;//array with N'
	//User interface
	Welcome();
	number_of_nodes = Read_Number_Nodes();
	n_prime = (char*)malloc(sizeof(char)*number_of_nodes);
	Check_memory_allocation(n_prime);
	input = Read_input(number_of_nodes);
	//Initialization
	computing_node = Create_Computing_Node(input, number_of_nodes);
	Print_Header_Dijksra_Table(computing_node.node, number_of_nodes);
	n_prime[0] = computing_node.name;
	Print_step(computing_node, number_of_nodes, step, n_prime);
	//Dijkstra
	for(step=1; step<number_of_nodes; step++)
	{
		current_node = Search_neighbor_min_d_cost(&computing_node, number_of_nodes, &n_prime[step]);
		for(i=0; i<(number_of_nodes-1); i++)
		{
			if(computing_node.node[i].minimum_cost==0)//only has to recompute if the cost is not determined to be minimum yet
			Recompute_cost_to_one_destination(&computing_node.node[i], current_node);
		}
		Print_step(computing_node, number_of_nodes, step, n_prime);
	}
	Free(computing_node, number_of_nodes);
	free(n_prime);
	return 0;
}

void Welcome()
{
	printf("\nWelcome to Dijkstra Resolver!.\n\n");
	/*
	*/
	printf("This program only allows to perform this algorithm for networks with 15 or less nodes."
	"The program would work just the same with more nodes but the table won't look as good.\n");
	printf("If your network has more than 6 nodes is better that you expand this window so that the table is seen properly.\n");
}

//Already implemented
int GetNumberFromString(char *argv)
{
	int i=0;
	int number = 0;
	int length = 0;

	for(i=0; argv[i]!='\0';i++);

	length = i;

	for(i=0; argv[i]!= '\0';i++)
	{
		if(isdigit(argv[i])!=0)
		{
			number += pow(10,length-i-1)*(argv[i]-'0');
		 } else
		 {
			 printf("\nError: All inputs are not numbers.\n");
			 return 0;
		 }
	}
	return number;
}

//Already implemented
int Read_Number_Nodes()
{
	int number_of_nodes;
	printf("\nHow many nodes are in the network?\n");
	number_of_nodes = Read_Number(2,15);
	return number_of_nodes;
}

//Already implemented
int Read_Number(int lower_limit, int upper_limit) //limits included in valid values
{
	int number;
	char flag=0; //flag to see if correct input
	char line[50];//used with fgets to read input from user
	do
	{
		fflush(stdin);
		fgets(line, sizeof(line), stdin);
		fflush(stdin);
		number = strtol(line, NULL, 10);
		if((number>upper_limit || number<lower_limit))
		{
			flag = 1;
			printf("Try again, put a value between %d and %d.\n", lower_limit,upper_limit) ;
		}else
		{
			flag = 0;
		}
	} while(flag);

	return number;
}

//Already implemented
char Read_Char()
{
	char character;
	char flag=0; //flag to see if correct input
	char line[5];//used with fgets to read input from user
	do
	{
		fflush(stdin);
		fgets(line, sizeof(line), stdin);
		fflush(stdin);
		character = line[0];
		if((character>='a' && character<='z') || (character>='A' && character<='Z'))
		{
			flag = 0;
		}else
		{
			flag = 1;
			printf("Try again, put a letter of the alphabet (lowercase or uppercase).\n");
		}
	} while(flag);

	return character;
}

//Already implemented
Node* Read_input(int number_of_nodes)
{
	/*
	Just reads all the nodes and creates an array of Node types
	But doesn't ask who is the computing node
	*/
	Node* input;
	int i=0, j=0;//counters
	char line[256];//to read input
	char flag_scanf;
	char* flag_fgets;
	char flag_input;
	char name;
	int cost;

	input=(Node*)malloc(sizeof(Node)*number_of_nodes);// creating a dynamic array for all the nodes input by the user.
  Check_memory_allocation(input);
	for(i=0;i<number_of_nodes;i++)
	{
		//Name of node
		printf("Enter the name of node %d \n",i+1);
		input[i].name = Read_Char();

		//Getting info of neighbors of this node
		printf("Please enter the neighbors of node %c in a way Node cost (example A 11).\n",input[i].name);
		printf("Enter one per line. To finish press Ctrl+Z in Windows or Ctrl+D in Unix.\n");
		printf("Neighbors of %c:\n",input[i].name);
		input[i].n_neighbors = 0;
		input[i].neighbor=(Neighbor*)malloc(sizeof(Neighbor));
		Check_memory_allocation(input[i].neighbor);
		for(j=0;;j++)//until EOF
		{
			do{
				fflush(stdin);
				flag_fgets = fgets(line, sizeof(line), stdin);
				fflush(stdin);
				flag_scanf = sscanf(line, "%c %d", &name, &cost);
				flag_input = (flag_scanf!=2 || !isalpha(name) || cost<0 || cost>32767)?1:0;
				if(flag_input) printf("Try again, the format is Name Cost (ex. A 11)(min cost is 0, max cost is 32767)\n");
			}while(flag_input);
			if(flag_fgets == NULL) break;//until EOF
			input[i].n_neighbors++;
			input[i].neighbor=(Neighbor*)realloc(input[i].neighbor, sizeof(Neighbor)*input[i].n_neighbors);
			Check_memory_allocation(input[i].neighbor);
			input[i].neighbor[j].neighbor = name;
			input[i].neighbor[j].cost = cost;
		}
		clearerr(stdin);
	}
	//just to check this function is OK, can be taken out
	for(i=0;i<number_of_nodes;i++)
	{
		printf("Node %c has %d neighbors (Node:cost):\n",input[i].name, input[i].n_neighbors);
		for(j=0;j<input[i].n_neighbors;j++)
		{
			printf("--%c: %d.\n",input[i].neighbor[j].neighbor, input[i].neighbor[j].cost);
		}
	}
	return input;
}

//Already implemented
Main_Node Create_Computing_Node(Node* input, int number_of_nodes)
{
	int i=0,j=0;//counters
	int pos;//position in input array of computing node
	int n_neighbors_main_node;//number of neighbors of computing node
	Neighbor* neighbors_main_node;//pointer to neighbors array of computing node
	char main_node_name;//name of computing node
	Main_Node computing_node;

	/*
	It has to ask the user who is the computing node
	then initializes a Main_Node with the array of nodes
	It has to initialize of the fields
	The node type with the name of the computing node is the main one,
	then the rest are used to populate the neighbor array with all the neighbors of this destination and n_neighbors,
	To represent an inifite cost is used the value 32767 (overflow of int of 2 bytes)
	input should be freed at the end
	*/

	//Creating array of destinations
	computing_node.node = (Destination*)malloc(sizeof(Destination)*(number_of_nodes-1));
	Check_memory_allocation(computing_node.node);
	if(computing_node.node == NULL)
	{
		printf("Error allocating dynamic memory.\n");
	}

	//Getting who is the computing node
	printf("Enter the Node which will be the main one.\n");
	main_node_name = Read_Char();
	computing_node.name= main_node_name;

	//Finding position of computing node in input array
	for(pos=0; pos<number_of_nodes; pos++)
	{
 	if(input[pos].name==main_node_name)
 	break;
	}

	//Getting info of computing node
	n_neighbors_main_node=input[pos].n_neighbors;//Get number of neighbors of computing node
	neighbors_main_node=input[pos].neighbor;//Get pointer to array of neighbors of computing node

	//Copying all nodes as destinations except the computing node
	for(i=0,j=0;i<(number_of_nodes-1);i++)
	{
		if(i==pos) //if we are at the position of the computing node, add 1 to what we read in input
		{
			j=1;
		}
		computing_node.node[i].name=input[i+j].name;
		computing_node.node[i].neighbor=input[i+j].neighbor;
		computing_node.node[i].n_neighbors=input[i+j].n_neighbors;
		computing_node.node[i].minimum_cost=0;
		computing_node.node[i].cost=32767;
		computing_node.node[i].previous_in_path='-';
	}

	// now we have to put the known values for its neighbors(of the main node)
	for(i=0;i<(number_of_nodes-1);i++) // Destination of the computing node;
	{
		for(j=0; j<n_neighbors_main_node; j++) // looking at the neighbors of the main node
		{
			if(computing_node.node[i].name==neighbors_main_node[j].neighbor)
			{
				computing_node.node[i].cost=neighbors_main_node[j].cost;
				computing_node.node[i].previous_in_path=main_node_name;
			}
		}
	}

	//just to check this function is OK, can be taken out
	printf("Computing node: %c:\n", computing_node.name);
	for(i=0;i<(number_of_nodes-1);i++)
	{
		printf("Name: %c, p(%c)=%c ", computing_node.node[i].name, computing_node.node[i].name, computing_node.node[i].previous_in_path);
		printf("D(%c)= %d, number of neighbors = %d \n", computing_node.node[i].name, computing_node.node[i].cost, computing_node.node[i].n_neighbors);
	}

	//freeing memory of input not used
	free(neighbors_main_node);
	free(input);
	return computing_node;
}

//Already implemented
int Min(int a, int b)
{
	int min;
	min = a>b?b:a;
	return min;
}

//Already implemented
void Print_Header_Dijksra_Table(Destination* node, int number_of_nodes)
{
	int i;
	int space = number_of_nodes*2 -2;
	printf("\nComputing Dijkstra:\n");
	printf(" Step |%*sN'%*s", (int)space/2, "", space-(int)space/2, "");
	for(i = 0; i<(number_of_nodes-1);i++)
	printf("| D(%c),p(%c)", node[i].name, node[i].name);
	printf("\n");
}

//Already implemented
void Print_step(Main_Node computing_node, int number_of_nodes, int step, char* n_prime)
{
	/*
	Do as in the Print_Header_Dijksra_Table but with the values for this current step
	For each printed
	if flag of minimum_cost is 1 -> only has to print "-"
	if cost is equal to 32767 has to say infinite
	*/

	int i;
	int space = number_of_nodes*2 -1 -step*2;
	int space2;
	int base10;
	int before;
	//Print Step
	printf("   %d  ", step);
	//Print N'
	printf("|");
	for(i = 0; i <=step; i++)
	{
		//printf("A"); //only until n_prime is actualized good
		printf("%c", n_prime[i]); //one for real use
		if(i < step) printf(",");
	}
	printf("%*s", space, "");

	space = 10 - 3; //in the other field minus comma, one letter and one digit

	for(i = 0; i < (number_of_nodes-1); i++)
	{
		if(computing_node.node[i].name != computing_node.name) // Skip the computing node
		{
			if(computing_node.node[i].minimum_cost == 1)
			{
				printf("|     -    ");      //Trying to maintain same length
			}
			else if(computing_node.node[i].cost >= 32767) // Infinite enough
			{
				printf("|    Inf   ");
			}
			else
			{
				base10 = (int) (log(computing_node.node[i].cost)/log(10));
				space2 =space - base10;
				before = space2-(int)space2/2;
				printf("|%*s%d,%c%*s", before, "",computing_node.node[i].cost, computing_node.node[i].previous_in_path, (int)space2/2, "");
			}
		}
	}

	printf("\n");
}

//Already implemented
Destination Search_neighbor_min_d_cost(Main_Node* computing_node, int number_of_nodes, char* n_prime)
{
	Destination current_node;
	/*
	Search among destinations with minimum_cost flag 0
	and find the one with less cost
	in that one update its minimum_cost flag to 1
	and return that destination (better if copy, not pointer)
	*/
	int i;
	int k;//place in internal bidimensional array where the one looked for is
	int size = 1;//size of the first index in bidimensinal array aka number of destination whose cost is not yet known to be minimum
	int the_min = 0;//flag used to find the minimum
	int **cost_number_array = NULL; // Bidimensional dynamic array to host the nodes with flag == 0

	//Searching who has flag == 0
	//Created double array with all elements whose cost is not known to be minimum yet
	//elements [x][0] are the position of the element in Destination array of Main_Node
	//elements [x][1] are the present cost of that position
	for(i = 0; i < (number_of_nodes-1); i++)
	{
		if(computing_node->node[i].minimum_cost == 0)
		{
			cost_number_array = (int **)realloc(cost_number_array, size*sizeof(int*));
			Check_memory_allocation(cost_number_array);

			size++;

			cost_number_array[size-2] = NULL;
			cost_number_array[size-2] = (int *)malloc(sizeof(int)*2);
			Check_memory_allocation(cost_number_array[size-2]);

			cost_number_array[size-2][0] = i;								// Position
			cost_number_array[size-2][1] = computing_node->node[i].cost;	// Cost
		}
	}

  //Finding who is the minimum
	//the approach taken is to compare each element with all the others
	//if it wasn't found any other who is less, then that's the minimum (or one of them)
	//this approach is good since even if there are two that are the minimum,
	//we only need (we choose the one we find earlier)
	for(k = 0; k < size-1; k++)
	{
		for(i = 0; i < size-1; i++)
		{
			if(cost_number_array[k][1] > cost_number_array[i][1])
			{
				the_min++;
			}
		}
		if(the_min == 0){ break; }
		the_min = 0;
	}

  //Performing operations in that destination with minimum cost
	computing_node->node[cost_number_array[k][0]].minimum_cost = 1;//setting minimum cost flag to 1, since now is knows is the minimum
	n_prime[0]= computing_node->node[cost_number_array[k][0]].name; //adding it to N'

	current_node = computing_node->node[cost_number_array[k][0]]; // returning this Destination to be used in main, This should be the one

	// Dobby is FREE!
	for(i = 0; i < size-1; i++)
	{
		free(cost_number_array[i]);
	}
	free(cost_number_array);

	return current_node;
}

//Already implemented
void Recompute_cost_to_one_destination(Destination* destination, Destination current_node)
{
	//destination only used with -> (since only reason for a pointer is to be able to modify it, not to loop the array)
	/*
	First checks that Neighbors array in current_node contains the name in destination, if not does nothing

	The new cost is the minimum (current cost in destination, sum of cost in current_node plus the cost in the neighbor in current_node that matches name in destination)

	if the cost has changed, update also the previous_in_path in destination to the name of current_node
	*/
	int i;//counter
	int old_cost;

	//Looking if new node with minimum cost has a path to this destination
	for(i=0; i<current_node.n_neighbors; i++)
	{
		if(destination->name == current_node.neighbor[i].neighbor)
		{
			old_cost = destination->cost;
			//If it has it, compute new cost
			destination->cost = Min(destination->cost, current_node.neighbor[i].cost + current_node.cost);

			//If cost has changed, update the previous node to it
			if(old_cost != destination->cost)
			{
				destination->previous_in_path= current_node.name;
			}
			return;//done, no need to keep searching, since there is only one neighbor that will match it (there is no two neighbors with same name (if user did OK...))
		}
	}
	return;//there is no neighbor with same name, so same cost
}

//Already implemented
void Free(Main_Node computing_node, int number_of_nodes)
{
	int i;
	for(i=0;i<(number_of_nodes-1);i++)
	{
		free(computing_node.node[i].neighbor);
	}
	free(computing_node.node);
}

//Already implemented
void Check_memory_allocation(void* a)
{
	if(a == NULL) { printf("ERROR allocating memory"); exit(0); }
}
