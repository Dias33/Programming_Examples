using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OBB_Collider : MonoBehaviour {

    public Vector3 Center_Of_Mass;
    public Vector3[] positionArray = new Vector3[3];
    public Vector3 extents;

    void Start ()
    {
        Center_Of_Mass = this.transform.position;
    }
	
	void Update ()
    {
        Center_Of_Mass = transform.position;
        positionArray[0] = transform.right; // x-axis vector
        positionArray[1] = transform.up; // y-axis vector
        positionArray[2] = transform.forward; // z-axis vector

        Vector3 min_extent = transform.position;
        Vector3 max_extent = transform.position;

        min_extent -= transform.localScale / 4;
        max_extent += transform.localScale / 4;

        extents = max_extent - min_extent;
    }
}
