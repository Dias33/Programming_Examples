using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * The grid is populated with nodes.
 * Each node has a radius (primarily used into detecting obstacles).
 */

public class Grid : MonoBehaviour 
{
	public Vector2 grid_world_size;
	public float node_radius;
	public LayerMask unwalkable_mask;

	Node[,] grid;
	float node_diameter;
	int grid_size_x, grid_size_y;

    public bool ShowGrid;
    public bool DisplayGridGizmos;

	void Awake()
	{
		node_diameter = node_radius * 2;
		grid_size_x = Mathf.RoundToInt (grid_world_size.x / node_diameter);
		grid_size_y = Mathf.RoundToInt (grid_world_size.y / node_diameter);
		Create_Grid ();
	}

    public int MaxSize
    {
        get { return grid_size_x * grid_size_y; }
    }

    /*
     * Create the grid and detects all obstacles on it.
     */

    public void Create_Grid()
	{
		grid = new Node[grid_size_x, grid_size_y];
		Vector3 world_bottom_left = transform.position - Vector3.right * grid_world_size.x/2 - Vector3.forward * grid_size_y/2;


		for (int x = 0; x < grid_size_x; x++) 
		{
			for (int y = 0; y < grid_size_y; y++) 
			{
				Vector3 world_point = world_bottom_left + Vector3.right * (x * node_diameter + node_radius) + Vector3.forward * (y * node_diameter + node_radius);
				bool walkable = !(Physics.CheckSphere (world_point, node_radius, unwalkable_mask)); // the check if there is a collision with an object (obstacle).
				grid [x, y] = new Node (walkable, world_point, x, y);
			}
		}
	}

    /*
     * Function that takes in a Vector3 position and locates it on a grid node.
     */
	public Node Node_From_World_Point(Vector3 world_position_)
	{
		float percent_x = (world_position_.x + grid_world_size.x / 2) / grid_size_x;
		float percent_y = (world_position_.z + grid_world_size.y / 2) / grid_size_y;
		percent_x = Mathf.Clamp01 (percent_x);
		percent_y = Mathf.Clamp01 (percent_y);

		int x = Mathf.RoundToInt((grid_size_x - 1) * percent_x);
		int y = Mathf.RoundToInt((grid_size_y - 1) * percent_y);      

		return grid [x, y];

	}

    /*
     * Returns a list of nodes that are the neighbours of the node given.
     */
	public List<Node> get_neighbours(Node node)
	{
		List<Node> neighbours = new List<Node> ();

		for (int x = -1; x <= 1; x++) 
		{
			for (int y = -1; y <= 1; y++) 
			{
				if (x == 0 && y == 0) 
				{
					continue;
				}

				int check_x = node.grid_x + x;
				int check_y = node.grid_y + y;

				if (check_x >= 0 && check_x < grid_size_x && check_y >= 0 && check_y < grid_size_y) 
				{
					neighbours.Add (grid [check_x, check_y]);
				}
			}
		}
		return neighbours;
	}

    /*
     * Visual debugging of the A* algorithm.
     */
	void OnDrawGizmos()
	{
        Gizmos.DrawWireCube(transform.position, new Vector3(grid_world_size.x, 1, grid_world_size.y));

        if (grid != null && DisplayGridGizmos)
        {
            foreach (Node n in grid)
            {
                Gizmos.color = (n.walkable) ? Color.white : Color.red;
                Gizmos.DrawCube(n.world_position, Vector3.one * (node_diameter - 0.1f));
            }
        }
        
	}
}
