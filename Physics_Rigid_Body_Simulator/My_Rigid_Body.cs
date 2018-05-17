using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class My_Rigid_Body : MonoBehaviour {

    public float gravity = -9.81f;
    public float mass = 0.0f;

    public float coeficient_restitution = 0.5f;
    public float coeficient_static_friction = 0.2f;
    public float coeficient_dynamic_friction = 0.3f;

    public Vector3 Velocity = Vector3.zero;
    public Vector3 Acceleration = Vector3.zero;
    public Vector3 Force = Vector3.zero;
    public Vector3 Correction_Position = Vector3.zero;

    public bool asleep_;
    public bool midair;

    void Start ()
    {
        Correction_Position = this.transform.position;

        if (this.gameObject.tag == "Plane")
        {
            asleep_ = true;
        }
        else
        {
            asleep_ = false;
        }

        midair = true;

    }
	
	void Update ()
    {
		
	}
}
