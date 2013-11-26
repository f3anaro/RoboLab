#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../h/routing.h"

node_t *__shortest_path_target = NULL;

void print_node (node_t *node);

void create_labyrinth (node_t **point_of_origin, node_list_t *list) {
	node_t* node_1 = node_create(-2,3);
	node_t* node_2 = node_create(-1,3);
	node_t* node_3 = node_create(0,3);
	node_t* node_4 = node_create(1,3);

	node_t* node_5 = node_create(-2,2);
	node_t* node_6 = node_create(-1,2);
	node_t* node_7 = node_create(0,2);
	node_t* node_8 = node_create(1,2);

	node_t* node_9  = node_create(-2,1);
	node_t* node_10 = node_create(-1,1);
	node_t* node_11 = node_create(0,1);
	node_t* node_12 = node_create(1,1);

	node_t* node_13 = node_create(-2,0);
	node_t* node_14 = node_create(-1,0);
	node_t* node_15 = node_create(0,0);
	node_t* node_16 = node_create(1,0);

	// insert nodes in list
	l_list_push(node_1 , list);
	l_list_push(node_2 , list);
	l_list_push(node_3 , list);
	l_list_push(node_4 , list);
	l_list_push(node_5 , list);
	l_list_push(node_6 , list);
	l_list_push(node_7 , list);
	l_list_push(node_8 , list);
	l_list_push(node_9 , list);
	l_list_push(node_10, list);
	l_list_push(node_11, list);
	l_list_push(node_12, list);
	l_list_push(node_13, list);
	l_list_push(node_14, list);
	l_list_push(node_15, list);
	l_list_push(node_16, list);

	// create labyrinth structure
	edge_create(node_1, node_2, 1);
	edge_create(node_2, node_3, 1);
	edge_create(node_3, node_4, 1);

	edge_create(node_1, node_5, 2);
	edge_create(node_2, node_6, 2);
	edge_create(node_4, node_8, 2);

	edge_create(node_6, node_7, 1);

	edge_create(node_7, node_11, 2);
	edge_create(node_8, node_12, 2);

	edge_create(node_9, node_10,  1);
	edge_create(node_10, node_11, 1);
	edge_create(node_11, node_12, 1);

	edge_create(node_9, node_13,  2);
	edge_create(node_11, node_15, 2);

	edge_create(node_14, node_15, 1);
	edge_create(node_15, node_16, 1);

	// set the point of origin
	*point_of_origin = node_15;
}

void print_locked (edge_t *edge) {
	if (edge->is_locked)
		printf("\t(locked)\n");
	else
		printf("\t(free)\n");
}

/*
 * Gibt einen Knoten auf STDOUT aus.
 */
void print_node (node_t *node) {
	printf("Node: (%d,%d)\n", node->x, node->y);
	printf("\tvisited:\t%d\n", node->visited);

	int i;
	for (i = 0; i < 4; i++) {
		edge_t *edge = node->edges[i];

		if (edge != NULL) {
			node_t *neighbour = edge_get_related_node(node, edge);
			printf("\t%d: (%d,%d)", i, neighbour->x, neighbour->y);
			print_locked(edge);
		} else {
			printf("\t%d: (empty)\n", i);
		}
	}
}

void print_list (l_list *list) {
	printf("\nNode List:\n");

	l_element *element = list->first;

	while(element != NULL) {
		print_node((node_t*) element->data);
		element = element->next;
	}
}

void print_route (l_list *route) {
	printf("Route:\n");

	if (route->size == 0) {
		printf("\tempty\n");
	}

	l_element *element = route->first;

	while (element != NULL) {
		switch (*((direction*) element->data)) {
			case 0:
				printf("\tNorth\n");
				break;
			case 1:
				printf("\tEast\n");
				break;
			case 2:
				printf("\tSouth\n");
				break;
			case 3:
				printf("\tWest\n");
				break;
			default:
				printf("\nUnknown direction: %d\n", *((direction*) element->data));
				break;
		}
		element = element->next;
	}
}


void walk (node_t **cnt_node, direction dir) {
	printf("Walk %d from (%d,%d) to ", dir, (*cnt_node)->x, (*cnt_node)->y);

	edge_t *edge = (*cnt_node)->edges[dir];

	if (edge == NULL) {
		printf("\nERROR: The node in direction %d from (%d,%d) does not exist", dir, (*cnt_node)->x, (*cnt_node)->y);
		return;
	}

	// mark node as visited
	edge->is_locked = 1;
	(*cnt_node) = edge_get_related_node(*cnt_node, edge);
	(*cnt_node)->visited = 1;

	printf("(%d,%d)\n", (*cnt_node)->x, (*cnt_node)->y);
}

int main(int argc, char **argv) {
	node_list_t *node_list = l_list_create();

	node_t* origin = NULL;
	create_labyrinth(&origin, node_list);
//	print_list(node_list);

	// int elements = node_list_destruct(list);
	// printf("%d elements was destructed\n", elements);

	// initialize random number generation
	srand(time(NULL));

	node_t *to = search_node_by_xy(-2,1, node_list);

	/*
	print_list(node_list);
	node_t *first_node = (node_t*) (l_list_pop(node_list))->data;
	print_list(node_list);
	printf("First node:");
	print_node(first_node);

	return 0;
	*/

	// print_node(to);

	/*if (to) {
		__shortest_path_target = to;
		route_t *route = shortest_path(origin, from_equals_to);
		// route_t *route = shortest_path(origin, current_node_has_unlocked_edges);

		print_route(route);
	}

	return 0;
	*/

	node_t *cnt_node = origin;

	int ready = 0;

	printf("Start with <Enter>");

	while (!ready) {
		// getchar();

		// mark current node as visited
		cnt_node->visited = 1;

		// local neighbours
		if (node_has_unvisited_neighbors(cnt_node)) {
//			printf("Local nodes\n");
			direction dir = select_random_direction(cnt_node);
			walk(&cnt_node, dir);
		}
		// global nodes
		else {
//			printf("Global nodes\n");
			l_list* route = shortest_path(cnt_node, callback_unvisited_neighbors);

			// Ready!
			if (route == NULL) {
				ready = 1;
			} else {
				l_element *element = route->first;

//				printf("Route:\n");
//
//				while (element != NULL) {
//					printf("%d\n", *((direction*) element->data));
//					element = element->next;
//				}
//				break;

				while (element != NULL) {
					direction dir = *((direction*) element->data);
					walk(&cnt_node, dir);
					element = element->next;
				}
				l_list_destruct(1, &route);
			}
		}
	}

	l_element *element = node_list->first;

	while (element != NULL) {
		l_element *next = element->next;
		node_t *node = (node_t*) element->data;
		element->data = NULL;

		// Knoten freigeben
		node_destruct(&node);

		// List-Element freigeben
		free(element);

		element = next;
	}
	free(node_list);

	return 0;
}
