 using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * Each node has a g-cost and an h-cost.
 * G-cost is a value that represents the distance from the starting node.
 * H-cost is a value that represents the distance from the end node.
 * F-cost = G-cost + H-cost
 */

public class Node : IHeapItem<Node>
{
	public bool walkable;
	public Vector3 world_position;
	public int grid_x;
	public int grid_y;

	public int g_cost;
	public int h_cost;
	public Node parent;
    int heapIndex;

	public Node (bool walkable_, Vector3 world_position_, int grid_x_, int grid_y_)
	{
		walkable = walkable_;
		world_position = world_position_;
		grid_x = grid_x_;
		grid_y = grid_y_;
	}

	public int f_Cost()
	{
		return g_cost + h_cost;
	}

    /*
    * Creates a reference Heap index of this node.
    * This allows each node to remember their index in the Heap data structure.
    */

    public int HeapIndex
    {
        get { return heapIndex; }
        set { heapIndex = value; }
    }

    /*
    * Compares two nodes based on their f-costs.
    */
    public int CompareTo(Node nodeToCompare)
    {
        int compare = f_Cost().CompareTo(nodeToCompare.f_Cost());

        if (compare == 0)
        {
            compare = h_cost.CompareTo(nodeToCompare.h_cost);
        }

        return -compare;
    }
}
