using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AABB_Collider : MonoBehaviour {

    public Vector3 MinExtent;
    public Vector3 MaxExtent;
    public Vector3 Center_Of_Mass = Vector3.zero;

    void Start ()
    {
        MinExtent = this.GetComponent<MeshRenderer>().bounds.min;
        MaxExtent = this.GetComponent<MeshRenderer>().bounds.max;
        Center_Of_Mass = this.transform.position;
    }
	
	void Update ()
    {
        MinExtent = this.GetComponent<MeshRenderer>().bounds.min;
        MaxExtent = this.GetComponent<MeshRenderer>().bounds.max;
        Center_Of_Mass = this.transform.position;
    }
}
