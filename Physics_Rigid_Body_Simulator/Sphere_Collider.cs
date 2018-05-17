using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Sphere_Collider : MonoBehaviour {

    public float Radius = 0.0f;
    public Vector3 Center_Of_Mass = Vector3.zero;

    void Start ()
    {
        Radius = this.GetComponent<MeshRenderer>().bounds.extents.x;
        Center_Of_Mass = this.transform.position;
    }
	

	void Update ()
    {
        Radius = this.GetComponent<MeshRenderer>().bounds.extents.x;
        Center_Of_Mass = this.transform.position;
    }
}
