#include <stdlib.h>
#include "../h/routing.h"
#include "../h/defines.h"

/* -----------------------------------------------------------------------
 * Implementation of a generic linked list
 * ----------------------------------------------------------------------- */

l_list *l_list_create () {
	l_list *list = (l_list*) malloc(sizeof(l_list));
	list->first = NULL;
	list->last = NULL;
	list->size = 0;
	return list;
}

void l_list_clear (int destructive, l_list *list) {
	// remove all elements from list
	l_element *element = list->first;

	if (destructive) {
		while (element != NULL) {
			l_element *tmp = element;
			element = element->next;

			if (tmp->data != NULL)
				free(tmp->data);
			free(tmp);
		}
	} else {
		while (element != NULL) {
			l_element *tmp = element;
			element = element->next;

			if (tmp->data != NULL)
				free(tmp);
		}
	}
}

void l_list_destruct (int destructive, l_list **list) {
	// do nothing if Null-Pointer
	if (list == NULL || *list == NULL)
		return;

	l_list_clear(destructive, *list);
	free(*list);
	*list = NULL;
}

l_element* l_list_push (void *data, l_list *list) {
	l_element *new_element = (l_element*) malloc(sizeof(l_element));
	new_element->data = data;
	new_element->next = NULL;

	// if the list is empty, set the element also as first element
	if (list->first == NULL) {
		list->first = new_element;
	}
	// link the last element to the new element
	else {
		list->last->next = new_element;
	}

	list->last = new_element;
	list->size++;
	return new_element;
}

l_element* l_list_pop (l_list *list) {
	l_element *first = list->first;

	if (first != NULL) {
		list->first = first->next;
		list->size--;
	}
	return first;
}

l_element* l_list_search (void *needle, l_list *haystack) {
	l_element *element = haystack->first;

	while (element != NULL) {
		if (element->data == needle)
			return element;
		element = element->next;
	}
	return NULL;
}

void l_list_reverse (l_list *list) {
	l_element *element = list->first;
	l_element *prev = NULL;
	l_element *tmp;

	// swap first and last element
	list->first = list->last;
	list->last = element;

	while (element != NULL) {
		tmp = element->next;
		element->next = prev;
		prev = element;
		element = tmp;
	}
}

/* -----------------------------------------------------------------------
 * Node Implementation
 * ----------------------------------------------------------------------- */

node_t* node_create (int x, int y) {
	node_t* pointer = (node_t*) malloc(sizeof(node_t));

	// definiere die Position des
	pointer->x = x;
	pointer->y = y;

	pointer->visited = 0;

	int i;
	for (i = 0; i < 4; i++) {
		pointer->edges[i] = NULL;
	}
	return pointer;
}

void node_destruct (node_t **node) {
	// destruct all edges
	int i;
	for(i = 0; i < 4; i++) {
		edge_t *edge = (**node).edges[i];

		if (edge != NULL) {
			node_t *related_node = edge_get_related_node(*node, edge);

			// clear pointer of the related node
			related_node->edges[OPPOSITE_DIR(i)] = NULL;
			free(edge);
		}
	}
	// free allocated memory of the node struct
	free(*node);

	// set node pointer to NULL
	*node = NULL;
}

int node_has_unlocked_edges (node_t *node) {
	int i;
	for(i = 0; i < 4; i++) {
		edge_t *edge = node->edges[i];

		if (edge != NULL && !edge->is_locked)
			return 1;
	}
	return 0;
}

int node_has_unvisited_neighbors (node_t *node) {
	int i;
	for(i = 0; i < 4; i++) {
		edge_t *edge = node->edges[i];

		if (edge != NULL && !(edge_get_related_node(node, edge))->visited) {
			return 1;
		}
	}
	return 0;
}

node_t *search_node_by_xy(int x, int y, l_list *node_list) {
	l_element *element = node_list->first;

	while (element != NULL) {
		node_t *node = (node_t*) element->data;

		if (node->x == x && node->y == y)
			return node;

		element = element->next;
	}

	return NULL;
}



edge_t* edge_create (node_t *from, node_t *to, direction dir) {
	edge_t *edge = (edge_t*) malloc(sizeof(edge_t));

	edge->is_locked = 0;
	edge->node_1 = from;
	edge->node_2 = to;

	// append edge to both nodes
	from->edges[dir] = edge;
	to->edges[OPPOSITE_DIR(dir)] = edge;

	return edge;
}

node_t* edge_get_related_node (node_t *node, edge_t *edge) {
	if (edge->node_1 == node)
		return edge->node_2;

	return edge->node_1;
}

predecessor* predecessor_create (node_t *node, node_t *pred_node, direction dir) {
	predecessor *pred = (predecessor*) malloc(sizeof(predecessor));
	pred->direction = dir;
	pred->node = node;
	pred->pred_node = pred_node;

	return pred;
}

predecessor* search_predecessor (node_t *node, l_list *list) {
	l_element *element = list->first;

	while (element != NULL) {
		predecessor *pred = (predecessor*) element->data;

		// found entry
		if (pred->node == node)
			return pred;
		element = element->next;
	}
	// no entry was found
	return NULL;
}

void route_push (direction dir, route_t *route) {
	direction* dir_ptr = (direction*) malloc(sizeof(direction));
	*dir_ptr = dir;
	l_list_push(dir_ptr, route);
}

int callback_from_equals_to (node_t *from, node_t *cnt_node) {
	if (cnt_node == __shortest_path_target) {
		__shortest_path_target = NULL;
		return 1;
	}
	return 0;
}

int callback_unvisited_neighbors (node_t *from, node_t *cnt_node) {
	if (node_has_unvisited_neighbors(cnt_node))
		return 1;
	return 0;
}


direction select_random_direction (node_t *node) {
	direction dir;

	while (1) {
		dir = rand() % 4;
		edge_t *edge = node->edges[dir];

		// check if the edge exists and the related node is not visited
		if (edge != NULL && !(edge_get_related_node(node, edge)->visited))
			break;
	}
	return dir;
}

route_t* shortest_path (node_t *from, condition_callback callback) {
	l_list *queue = l_list_create();
	l_list *visited_nodes = l_list_create();
	l_list *pred_list = l_list_create();
	l_list *route = NULL;

	l_list_push(from, queue);
	l_list_push(from, visited_nodes);

	while (queue->size != 0) {
		node_t *cnt_node = (node_t*) (l_list_pop(queue))->data;
		// mark current node as visited
		l_list_push(cnt_node, visited_nodes);

		// found path
		if (callback(from, cnt_node)) {
			// create linked list
			route = l_list_create();

			while (cnt_node != from) {
				predecessor *pred = search_predecessor(cnt_node, pred_list);

				// print predecessor
				route_push(pred->direction, route);
				cnt_node = pred->pred_node;
			}

			l_list_reverse(route);
			break;
		}

		// append each unvisited child node to the queue randomly
		int i;
		for(i = 0; i < 4; i++) {
			edge_t *edge = cnt_node->edges[i];

			// if edge does not exist continue
			if (edge == NULL)
				continue;

			node_t *child_node = edge_get_related_node(cnt_node, edge);

			// if the child not was not visited yet, append it to the queue
			if (!l_list_search(child_node, visited_nodes)) {
				// create predecessor element
				predecessor *pred = predecessor_create(child_node, cnt_node, i);
				// push queue
				l_list_push(child_node, queue);
				// push predecessor list
				l_list_push(pred, pred_list);
			}
		}
	}
	// free allocated memory
	l_list_destruct(0, &queue);
	l_list_destruct(1, &pred_list);
	l_list_destruct(0, &visited_nodes);

	return route;
}
