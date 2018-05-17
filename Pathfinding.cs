using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

 /* A* Pathfinding -> Based on a grid with nodes.
 * 
 * Each node has a g-cost and an h-cost.
 * G-cost is a value that represents the distance from the starting node.
 * H-cost is a value that represents the distance from the end node.
 * F-cost = G-cost + H-cost
 * 
 * F-cost is used to determine which next node should the algorithm follow in order to produce an optimal path to the end node.
 * This is done by simply comparing all the surrouding nodes f-cost against one another and will choose the lowest f-cost.
 * All surrouding walkable node's values are stored to be compared for later.
 * If there are 2 or more nodes with the same f-cost, the algorithm will pick the node with the lowest h-cost.
 * 
 * Each unit that wants to use the pathfinding function must request it from the manager -> see PathRequestManager script.
 * This algorithm is optimized with a heap implementation -> see Heap script.
 */ 

public class Pathfinding : MonoBehaviour 
{

    PathRequestManager requestManager;
	Grid grid;

	void Awake()
    {
        requestManager = GetComponent<PathRequestManager>();
        grid = GetComponent<Grid> ();
	}

    public void StartFindPath(Vector3 startPos, Vector3 targetPos)
    {
        StartCoroutine(Find_Path(startPos, targetPos));
    }

	IEnumerator Find_Path(Vector3 start_position, Vector3 target_position) // Calculates the shortest path from the a given start point to a given end point.
	{
		Node start_node = grid.Node_From_World_Point (start_position);
		Node target_node = grid.Node_From_World_Point (target_position);

        Vector3[] waypoints = new Vector3[0]; // vector to store the complete path to be sent to each unit who uses the pathfinding algorithm.
        bool pathSuccess = false;

		Heap<Node> open_set = new Heap<Node>(grid.MaxSize); // set of nodes to be evaluated.
		HashSet<Node> closed_set = new HashSet<Node> (); // set of nodes already evaluated.

        if (start_node.walkable && target_node.walkable)
        {
            open_set.Add(start_node);

            while (open_set.Count > 0)
            {
                Node current_node = open_set.RemoveFirst();
                closed_set.Add(current_node);

                if (current_node == target_node)
                {
                    pathSuccess = true;
                    break;
                }

                foreach (Node neighbour in grid.get_neighbours(current_node))
                {
                    if (!neighbour.walkable || closed_set.Contains(neighbour)) // if neighbour is not traversable or neighbour is in the closed set.
                    {
                        continue;
                    }

                    int new_movement_cost_to_neighbour = current_node.g_cost + Get_Distance(current_node, neighbour);

                    if (new_movement_cost_to_neighbour < neighbour.g_cost || !open_set.Contains(neighbour))
                    {
                        neighbour.g_cost = new_movement_cost_to_neighbour;
                        neighbour.h_cost = Get_Distance(neighbour, target_node);
                        neighbour.parent = current_node;

                        if (!open_set.Contains(neighbour))
                        {
                            open_set.Add(neighbour);
                        }
                        else
                        {
                            open_set.UpdateItem(neighbour);
                        }
                    }
                }
            }
        }
        yield return null;

        if (pathSuccess)
        {
            waypoints = Retrace_Path(start_node, target_node);
        }

        requestManager.FinishedProcessingPath(waypoints, pathSuccess); // sends the calculated path to the path manager.
	}

    /* 
    * The calculated path is stored in reverse (from the end point towards the start point).
    *  Retrace_Path function reverses that to be from the start point to the end point.
    */

    Vector3[] Retrace_Path(Node start_node, Node end_node) 
	{
		List<Node> path = new List<Node> ();
		Node current_node = end_node;

		while (current_node != start_node) 
		{
			path.Add (current_node);
			current_node = current_node.parent;
		}

        Vector3[] waypoints = SimplifyPath(path);
        Array.Reverse(waypoints);

        return waypoints;	
	}

    /*
    * The calculated path may contain straight lines across multiple nodes.
    * Simply_Path function reduces the number of waypoints that each unit needs to go through.
    * Therefore, the final path will now only store the waypoints in which the direction needs to change.
    */
    Vector3[] SimplifyPath(List<Node> path)
    {
        List<Vector3> waypoints = new List<Vector3>();
        Vector2 direction_old = Vector2.zero;

        for (int i = 1; i < path.Count; i++)
        {
            Vector2 directionNew = new Vector2(path[i - 1].grid_x - path[i].grid_x, path[i - 1].grid_y - path[i].grid_y);
            if (directionNew != direction_old)
            {
                waypoints.Add(path[i].world_position);
                direction_old = directionNew;
            }
        }

        return waypoints.ToArray();
    }

     /*
     * Calculates the distance from one node to another based on the distance between them and their f-costs.
     */
	int Get_Distance(Node node_a, Node node_b)
	{
		int distance_x = Mathf.Abs (node_a.grid_x - node_b.grid_x);
		int distance_y = Mathf.Abs (node_a.grid_y - node_b.grid_y);

		if (distance_x > distance_y) 
		{
			return 14 * distance_y + 10 * (distance_x - distance_y);
		}

		return 14 * distance_x + 10 * (distance_y - distance_x);

	}

}
