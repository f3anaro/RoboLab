#ifndef ROUTING_H_
#define ROUTING_H_

/* -----------------------------------------------------------------------
 * Generic linked list
 * ----------------------------------------------------------------------- */

typedef struct l_element l_element;

// generic linked list element
struct l_element {
	l_element *next;
	void *data;
};

typedef struct l_list l_list, node_list_t, route_t;

// generic head of a linked list
struct l_list {
	// Pointer to the first element of the list
	l_element *first;

	// Pointer to the last element of the list
	l_element *last;

	// number of elements in the list
	int size;
};

/*
 * Constructor of a linked list
 */
l_list* l_list_create ();

/*
 * Destructor of the linked list. If destructive is 0, the elements will not be freed.
 * Otherwise all elements will be freed.
 */
void l_list_destruct (int destructive, l_list **list);

/*
 * Removes all elements form the list. The elements will not be freed if "destructive" is
 * set to 0.
 */
void l_list_clear (int destructive, l_list *list);

/*
 * Inserts a new element at the end of the linked list
 */
l_element* l_list_push (void *data, l_list *list);

/*
 * Removes the first element from a linked list and returns it.
 */
l_element* l_list_pop (l_list *list);

/*
 * Searches and returns an element in the linked list and return it if
 * it was found.
 */
l_element* l_list_search (void *needle, l_list *haystack);

/*
 * Reverts the order of the linked list.
 */
void l_list_reverse (l_list *list);

/*
 * Data-Type for absolute directions. They are the only route
 * indicator.
 */
typedef enum direction {
	NORTH,
	EAST,
	SOUTH,
	WEST
} direction;

#define OPPOSITE_DIR(dir) (dir + 2) % 4

/* -----------------------------------------------------------------------
 * Node and Edge Definition and Functions
 * ----------------------------------------------------------------------- */

typedef struct node_t node_t;
typedef struct edge_t edge_t;

/*
 * Central data object in the program. A node represents a crossover in the
 * maze.
 */
struct node_t {
	/*
	 * This two integers identify the node unique by its position in the
	 * maze grid
	 */
	int x;
	int y;

	edge_t *edges[4];

	/*
	 * Identifies if the node was already visited or not.
	 */
	int visited;
};

/*
 * Struct of an edge. An edge joins to nodes and has the information
 * if it was already visited or not.
 */
struct edge_t {
	node_t *node_1;
	node_t *node_2;

	int is_locked;
};

/*
 * Function to create a node. All "passed" values will be set to "false"
 * and the edges to north, east, south and west will be set to "NULL"
 * It returns a pointer to the new created node
 */
node_t* node_create (int x, int y);

/*
 * Destructor of a node. All edges to other nodes will also be destructed.
 */
void node_destruct (node_t **node);

node_t *search_node_by_xy(int x, int y, l_list *node_list);

void print_node (node_t *node);

/*
 * Constructor of an edge between two nodes. The edge is undirected, but the direction
 * is needed to set the pointer of the nodes to the edge correctly.
 */
edge_t* edge_create (node_t *from, node_t *to, direction dir);

/*
 * Destructor of an edge
 */
void edge_destruct (edge_t **edge);

/*
 * Selects the related node of another node.
 */
node_t* edge_get_related_node(node_t *node, edge_t *edge);

/*
 * Check if a node has one or more edges the NXT has not passed yet.
 */
int node_has_unlocked_edges (node_t *node);

/*
 * Check if a node as neighbour nodes the NXT has not visited yet.
 */
int node_has_unvisited_neighbors (node_t *node);

/* -----------------------------------------------------------------------
 * Breadth first search
 * ----------------------------------------------------------------------- */

/*
 * Struct for storing the order in the breadth first search. For each node
 * the predecessor is stored. n this way the route from one node to another
 * can be reconstructed.
 */
typedef struct predecessor predecessor;

struct predecessor {
	node_t *node;
	node_t *pred_node;
	direction direction;
};

/*
 * Callback function pointer for breadth first search cancel decision.
 */
typedef int (*condition_callback)(node_t *from, node_t *cnt_node);

/*
 * Cancel condition for breadth first search. The condition for a node is
 * that it points to a neighbor node that is unvisited.
 */
int callback_unvisited_neighbors (node_t *from, node_t *cnt_node);

/*
 * Global variable for search the shortest path from one to to another
 * this variable stores the target node ("to")
 */
extern node_t *__shortest_path_target;

/*
 * Cancel condition callback function for the breadth first search algorithm.
 * If a the scanned node equals the "__shortest_path_target" node, the search
 * succeed.
 */
int callback_from_equals_to (node_t *from, node_t *cnt_node);

/*
 * Calculates the shortest known path between the two points. The path
 * is returned as a route
 */
route_t* shortest_path (node_t *from, condition_callback callback);

/*
 * Selects a random direction.
 */
direction select_random_direction (node_t *node);

#endif /* ROUTING_H_ */
