using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Scene_Manager : MonoBehaviour {

    Collisions Collisions_Manager = null;
    Objects_Manager Objects_Manager = null;
    OBB_To_OBB_Collision OBB_Collision = null;
    public bool paused;
    public bool obb_scene;

    void Start ()
    {
        Collisions_Manager = this.GetComponent<Collisions>();
        Objects_Manager = this.GetComponent<Objects_Manager>();
        OBB_Collision = this.GetComponent<OBB_To_OBB_Collision>();
	}
	
	void FixedUpdate ()
    {
        if (Input.GetKeyDown(KeyCode.P))
        {
            if (paused)
            {
                paused = false;
            }
            else
            {
                paused = true;
            }
        }

        if (Input.GetKey(KeyCode.Space) && paused)
        {
            if (obb_scene == false)
            {
                Objects_Manager.Object_Movement();
                Collisions_Manager.Solve_Collisions();
            }
            else
            {
                Objects_Manager.Object_Movement();
                OBB_Collision.OBB_Method();
            }

        }

        if (!paused)
        {
            if (obb_scene == false)
            {
                Objects_Manager.Object_Movement();
                Collisions_Manager.Solve_Collisions();
            }
            else
            {
                Objects_Manager.Object_Movement();
                OBB_Collision.OBB_Method();
            }
        }

    }
}
