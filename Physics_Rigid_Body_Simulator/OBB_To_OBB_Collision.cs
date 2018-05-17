using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OBB_To_OBB_Collision : MonoBehaviour {


	void Start ()
    {
		
	}
	

	void Update ()
    {
		
	}

    public void OBB_Method()
    {
        OBB_Collider[] boxColliders = (OBB_Collider[])GameObject.FindObjectsOfType(typeof(OBB_Collider));

        // Loop for checking all AABB and OBB against other AABB and OBB

        for (int i = 0; i < boxColliders.Length; i++)
        {
            OBB_Collider First_Box = boxColliders[i];

            for (int j = 1; j < boxColliders.Length; j++)
            {
                if (j > i)
                {
                    OBB_Collider Second_Box = boxColliders[j];

                    if (!(First_Box.GetComponent<My_Rigid_Body>().asleep_ == true && Second_Box.GetComponent<My_Rigid_Body>().asleep_ == true))
                    {
                        if (First_Box.GetComponent<My_Rigid_Body>().mass != 0 || Second_Box.GetComponent<My_Rigid_Body>().mass != 0
                            || Second_Box.GetComponent<My_Rigid_Body>().mass != 0 || First_Box.GetComponent<My_Rigid_Body>().mass != 0)
                        {
                            if (OBB_to_OBB(First_Box.GetComponent<OBB_Collider>(), Second_Box.GetComponent<OBB_Collider>()))
                            {
                                Debug.Log("Works");
                                First_Box.GetComponent<My_Rigid_Body>().asleep_ = true;
                                Second_Box.GetComponent<My_Rigid_Body>().asleep_ = true;
                            }
                        }
                    }
                }
            }
        }
    }

    public bool OBB_to_OBB(OBB_Collider A, OBB_Collider B)
    {
        float ra, rb;
        Matrix4x4 R = Matrix4x4.identity;
        Matrix4x4 AbsR = Matrix4x4.identity;

        // Compute rotation matrix expressing b in a’s coordinate frame
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                R[i, j] = Vector3.Dot(A.positionArray[i], B.positionArray[j]);
            }

        }

        // Compute translation vector t
        Vector3 t = B.Center_Of_Mass - A.Center_Of_Mass;
        // Bring translation into a’s coordinate frame

        t = new Vector3(Vector3.Dot(t, A.positionArray[0]), Vector3.Dot(t, A.positionArray[1]), Vector3.Dot(t, A.positionArray[2]));
        // Compute common subexpressions. Add in an epsilon term to 
        // counteract arithmetic errors when two edges are parallel and
        // their cross product is (near) null (see text for details)

        float EPSILON = 0.001f;

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                AbsR[i, j] = Mathf.Abs(R[i, j]) + EPSILON;

        // Test axes L = A0, L = A1, L = A2
        for (int i = 0; i < 3; i++)
        {
            ra = A.extents[i];
            rb = B.extents[0] * AbsR[i, 0] + B.extents[1] * AbsR[i, 1] + B.extents[2] * AbsR[i, 2];
            if (Mathf.Abs(t[i]) > ra + rb)
            { return false; }
        }

        // Test axes L = B0, L = B1, L = B2
        for (int i = 0; i < 3; i++)
        {
            ra = A.extents[0] * AbsR[0, i] + A.extents[1] * AbsR[1, i] + A.extents[2] * AbsR[2, i];
            rb = B.extents[i];
            if (Mathf.Abs(t[0] * R[0, i] + t[1] * R[1, i] + t[2] * R[2, i]) > ra + rb)
            { return false; }
        }

        // Test axis L = A0 x B0
        ra = A.extents[1] * AbsR[2, 0] + A.extents[2] * AbsR[1, 0];
        rb = B.extents[1] * AbsR[0, 2] + B.extents[2] * AbsR[0, 1];
        if (Mathf.Abs(t[2] * R[1, 0] - t[1] * R[2, 0]) > ra + rb)
        { return false; }

        // Test axis L = A0 x B1
        ra = A.extents[1] * AbsR[2, 1] + A.extents[2] * AbsR[1, 1];
        rb = B.extents[0] * AbsR[0, 2] + B.extents[2] * AbsR[0, 0];
        if (Mathf.Abs(t[2] * R[1, 1] - t[1] * R[2, 1]) > ra + rb)
        { return false; }

        // Test axis L = A0 x B2
        ra = A.extents[1] * AbsR[2, 2] + A.extents[2] * AbsR[1, 2];
        rb = B.extents[0] * AbsR[0, 1] + B.extents[1] * AbsR[0, 0];
        if (Mathf.Abs(t[2] * R[1, 2] - t[1] * R[2, 2]) > ra + rb)
        { return false; }

        // Test axis L = A1 x B0
        ra = A.extents[0] * AbsR[2, 0] + A.extents[2] * AbsR[0, 0];
        rb = B.extents[1] * AbsR[1, 2] + B.extents[2] * AbsR[1, 1];
        if (Mathf.Abs(t[0] * R[2, 0] - t[2] * R[0, 0]) > ra + rb)
        { return false; }

        // Test axis L = A1 x B1
        ra = A.extents[0] * AbsR[2, 1] + A.extents[2] * AbsR[0, 1];
        rb = B.extents[0] * AbsR[1, 2] + B.extents[2] * AbsR[1, 0];
        if (Mathf.Abs(t[0] * R[2, 1] - t[2] * R[0, 1]) > ra + rb)
        { return false; }

        // Test axis L = A1 x B2
        ra = A.extents[0] * AbsR[2, 2] + A.extents[2] * AbsR[0, 2];
        rb = B.extents[0] * AbsR[1, 1] + B.extents[1] * AbsR[1, 0];
        if (Mathf.Abs(t[0] * R[2, 2] - t[2] * R[0, 2]) > ra + rb)
        { return false; }

        // Test axis L = A2 x B0
        ra = A.extents[0] * AbsR[1, 0] + A.extents[1] * AbsR[0, 0];
        rb = B.extents[1] * AbsR[2, 2] + B.extents[2] * AbsR[2, 1];
        if (Mathf.Abs(t[1] * R[0, 0] - t[0] * R[1, 0]) > ra + rb)
        { return false; }

        // Test axis L = A2 x B1
        ra = A.extents[0] * AbsR[1, 1] + A.extents[1] * AbsR[0, 1];
        rb = B.extents[0] * AbsR[2, 2] + B.extents[2] * AbsR[2, 0];
        if (Mathf.Abs(t[1] * R[0, 1] - t[0] * R[1, 1]) > ra + rb)
        { return false; }

        // Test axis L = A2 x B2
        ra = A.extents[0] * AbsR[1, 2] + A.extents[1] * AbsR[0, 2];
        rb = B.extents[0] * AbsR[2, 1] + B.extents[1] * AbsR[2, 0];
        if (Mathf.Abs(t[1] * R[0, 2] - t[0] * R[1, 2]) > ra + rb)
        { return false; }

        // Since no separating axis is found, the OBBs must be intersecting
        return true;

    }
}
