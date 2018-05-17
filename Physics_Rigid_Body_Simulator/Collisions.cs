using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Collisions : MonoBehaviour {

    public Vector3 normal_point_of_contact;
    public float penetration;

    void Start ()
    {
		
	}
	
	void Update ()
    {
		
	}

    public void Solve_Collisions()
    {
        AABB_Collider[] boxColliders = (AABB_Collider[])GameObject.FindObjectsOfType(typeof(AABB_Collider));

        Sphere_Collider[] sphereColliders = (Sphere_Collider[])GameObject.FindObjectsOfType(typeof(Sphere_Collider));

        // Loop for checking all AABB and OBB against other AABB and OBB

        for (int i = 0; i < boxColliders.Length; i++)
        {
            AABB_Collider First_Box = boxColliders[i];

            for (int j = 1; j < boxColliders.Length; j++)
            {
                if (j > i)
                {
                    AABB_Collider Second_Box = boxColliders[j];

                    if (!(First_Box.GetComponent<My_Rigid_Body>().asleep_ == true && Second_Box.GetComponent<My_Rigid_Body>().asleep_ == true))
                    {
                        if (First_Box.GetComponent<My_Rigid_Body>().mass != 0 || Second_Box.GetComponent<My_Rigid_Body>().mass != 0 
                            || Second_Box.GetComponent<My_Rigid_Body>().mass != 0 || First_Box.GetComponent<My_Rigid_Body>().mass != 0)
                        {
                            if (AABB_VS_AABB(First_Box, Second_Box))
                            {
                                if (OBB_to_OBB(First_Box.GetComponent<OBB_Collider>(), Second_Box.GetComponent<OBB_Collider>()))
                                {
                                    Impulse_for_Two(First_Box.GetComponent<My_Rigid_Body>(), Second_Box.GetComponent<My_Rigid_Body>());
                                }
                            }
                        }
                    }
                }
            }

            // Loop for all AABB and OBB against spheres

            for (int k = 0; k < sphereColliders.Length; k++)
            {
                Sphere_Collider Sphere = sphereColliders[k];

                if (!(First_Box.GetComponent<My_Rigid_Body>().asleep_ == true && Sphere.GetComponent<My_Rigid_Body>().asleep_ == true))
                {
                    if (AABB_VS_Sphere(First_Box, Sphere))
                    {
                        //Debug.Log("Hit");
                        Impulse_for_Two(First_Box.GetComponent<My_Rigid_Body>(), Sphere.GetComponent<My_Rigid_Body>());
                    }
                }
            }
        }

        // Loop for Sphere gainst Sphere

        for (int i = 0; i < sphereColliders.Length; i++)
        {
            Sphere_Collider sphere = sphereColliders[i];

            for (int j = 1; j < sphereColliders.Length; j++)
            {
                if (j > i)
                {
                    Sphere_Collider otherSphere = sphereColliders[j];

                    if (!(sphere.GetComponent<My_Rigid_Body>().asleep_ == true && otherSphere.GetComponent<My_Rigid_Body>().asleep_ == true))
                    {
                        if (Sphere_Vs_Sphere(sphere, otherSphere))
                        {
                            Impulse_for_Two(sphere.GetComponent<My_Rigid_Body>(), otherSphere.GetComponent<My_Rigid_Body>());
                        }
                    }
                }
            }
        }
    }

    public void Impulse_for_Two(My_Rigid_Body A, My_Rigid_Body B)
    {
        //Relative Velocity

        Vector3 RV = B.Velocity - A.Velocity;

        float VelocityAlongNormal = Vector3.Dot(RV, normal_point_of_contact);

        if (VelocityAlongNormal > 0)
        {
            return;
        }

        // Choose lowest restitution

        float e = Mathf.Min(A.coeficient_restitution, B.coeficient_restitution);

        // Calculate Impulse Scalar

        float J = -(1.0f + e) * VelocityAlongNormal;
        J /= (1.0f / A.mass) + (1.0f / B.mass);

        // Apply Impulse

        Vector3 Impulse = J * normal_point_of_contact;
        Vector3 New_Force_A = (1.0f / A.mass) * Impulse;
        Vector3 New_Force_B = (1.0f / B.mass) * Impulse;

        float sleep_threshold = 0.05f;

        if (A.asleep_ == true)
        {
            if (A.mass != 0)
            {
                if ((New_Force_A.x > sleep_threshold || New_Force_A.y > sleep_threshold || New_Force_A.z > sleep_threshold))
                {
                    A.asleep_ = false;
                    A.Velocity -= (1.0f / A.mass) * Impulse;
                    A.gameObject.GetComponent<Renderer>().material.color = Color.red;
                }
            }
        }
        else
        {
            if (A.mass != 0)
            {
                A.Velocity -= (1.0f / A.mass) * Impulse;
            }
        }

        if (B.asleep_ == true)
        {
            if (B.mass != 0)
            {
                if ((New_Force_B.x > sleep_threshold || New_Force_B.y > sleep_threshold || New_Force_B.z > sleep_threshold))
                {
                    B.asleep_ = false;
                    B.Velocity += (1.0f / B.mass) * Impulse;
                    B.gameObject.GetComponent<Renderer>().material.color = Color.red;
                }
            }
        }
        else
        {
            if (B.mass != 0)
            { 
                B.Velocity += (1.0f / B.mass) * Impulse;
            }
        }

        Impulse_Correction_for_Two(A, B);

        if (A.gameObject.tag == "Plane" || A.gameObject.tag == "Wall")
        {
            Plane_Friction(B);
        }

        else if (B.gameObject.tag == "Plane" || B.gameObject.tag == "Wall")
        {
            Plane_Friction(A);
        }
        else
        {
            Friction(A, B, J);
        }

        if (A.gameObject.tag == "Plane")
        {
            B.midair = false;
        }

        else if (B.gameObject.tag == "Plane")
        {
            A.midair = false;
        }


    }

    public void Impulse_Correction_for_Two(My_Rigid_Body A, My_Rigid_Body B)
    {
        float Percent = 0.1f;
        float Threshhold = 0.03f;

        Vector3 correction = Mathf.Max(penetration - Threshhold, 0.0f) / (1.0f / A.mass + 1.0f / B.mass) * Percent * normal_point_of_contact;

        if (A.mass != 0)
        {
            A.Correction_Position -= 1.0f / A.mass * correction;
        }

        if (B.mass != 0)
        {
            B.Correction_Position += 1.0f / B.mass * correction;
        }
    }

    public void Friction(My_Rigid_Body A, My_Rigid_Body B, float j)
    {
        float A_coefficient_static_friction = A.coeficient_static_friction;
        float B_coefficient_static_friction = B.coeficient_static_friction;

        float A_coefficient_dynamic_friction = A.coeficient_dynamic_friction;
        float B_coefficient_dynamic_friction = B.coeficient_dynamic_friction;

        Vector3 RV = B.Velocity - A.Velocity;

        // Tangent vector
        Vector3 tangent = RV - Vector3.Dot(RV, normal_point_of_contact) * normal_point_of_contact;
        tangent.Normalize();

        // Solve for magnitude to apply along the friction vector
        float jt = -Vector3.Dot(RV, tangent);
        jt = jt / ((1.0f / A.mass) + (1.0f / B.mass));

        float mu = Mathf.Max(A_coefficient_static_friction, B_coefficient_static_friction);

        // Clamp magnitude of friction and create impulse vector
        Vector3 frictionImpulse;

        if (Mathf.Abs(jt) < j * mu)
        {
            frictionImpulse = jt * tangent;
        }
        else
        {
            float dynamicFriction = Mathf.Max(A_coefficient_dynamic_friction, B_coefficient_dynamic_friction);
            frictionImpulse = -j * tangent * dynamicFriction;
        }

        A.Velocity -= (1.0f / A.mass) * frictionImpulse;

        B.Velocity += (1.0f / B.mass) * frictionImpulse;

    }

    public bool Sphere_Vs_Sphere(Sphere_Collider A, Sphere_Collider B)
    {
        Vector3 A_pos = A.transform.position;
        Vector3 B_pos = B.transform.position;

        Vector3 N = B_pos - A_pos;
        float R = A.Radius + B.Radius;

        if (N.sqrMagnitude > R * R)
        {
            return false;
        }

        float D = N.magnitude;

        if (D != 0)
        {
            penetration = R - D;
            normal_point_of_contact = N / D;
            return true;
        }
        else
        {
            penetration = A.Radius;
            normal_point_of_contact = new Vector3(1, 0, 0);
            return true;
        }

    }

    public bool AABB_VS_AABB(AABB_Collider A, AABB_Collider B)
    {
        Vector3 N = B.Center_Of_Mass - A.Center_Of_Mass;

        float axExtents = (A.MaxExtent.x - A.MinExtent.x) / 2;
        float bxExtents = (B.MaxExtent.x - B.MinExtent.x) / 2;

        float xOverlap = axExtents + bxExtents - Mathf.Abs(N.x);

        if (xOverlap > 0)
        {
            float ayExtents = (A.MaxExtent.y - A.MinExtent.y) / 2;
            float byExtents = (B.MaxExtent.y - B.MinExtent.y) / 2;

            float yOverlap = ayExtents + byExtents - Mathf.Abs(N.y);

            if (yOverlap > 0)
            {
                float azExtents = (A.MaxExtent.z - A.MinExtent.z) / 2;
                float bzExtents = (B.MaxExtent.z - B.MinExtent.z) / 2;

                float zOverlap = azExtents + bzExtents - Mathf.Abs(N.z);

                if (zOverlap > 0)
                {
                    if (xOverlap < yOverlap && xOverlap < zOverlap)
                    {
                        if (N.x < 0)
                        {
                            normal_point_of_contact = new Vector3(-1, 0, 0);
                        }
                        else
                        {
                            normal_point_of_contact = new Vector3(1, 0, 0); 
                        }

                        penetration = xOverlap;

                        return true;
                    }
                    if (yOverlap < zOverlap && yOverlap < xOverlap)
                    {
                        if (N.y < 0)
                        {
                            normal_point_of_contact = new Vector3(0, -1, 0);
                        }
                        else
                        {
                            normal_point_of_contact = new Vector3(0, 1, 0);
                        }

                        penetration = yOverlap;

                        return true;

                    }
                    if(zOverlap < yOverlap && zOverlap < xOverlap)
                    {
                        if (N.z < 0)
                        {
                            normal_point_of_contact = new Vector3(0, 0, -1);
                        }
                        else
                        {
                            normal_point_of_contact = new Vector3(0, 0, 1);
                        }

                        penetration = zOverlap;

                        
                        return true;

                    }
                }
            }
        }
        return false;
    }

    public bool AABB_VS_Sphere(AABB_Collider AABB, Sphere_Collider Sphere)
    {
        Vector3 AABB_Position = AABB.transform.position;
        Vector3 Sphere_Position = Sphere.transform.position;

        Vector3 N = Sphere_Position - AABB_Position;

        Vector3 closest = N;

        float x_extent = (AABB.MaxExtent.x - AABB.MinExtent.x) / 2;
        float y_extent = (AABB.MaxExtent.y - AABB.MinExtent.y) / 2;
        float z_extent = (AABB.MaxExtent.z - AABB.MinExtent.z) / 2;

        closest.x = Mathf.Clamp(closest.x, -x_extent, x_extent);
        closest.y = Mathf.Clamp(closest.y, -y_extent, y_extent);
        closest.z = Mathf.Clamp(closest.z, -z_extent, z_extent);

        bool inside = false;

        if (N == closest)
        {
            inside = true;

            if (Mathf.Abs(N.x) > Mathf.Abs(N.y) && Mathf.Abs(N.x) > Mathf.Abs(N.z))
            {
                if (closest.x > 0)
                {
                    closest.x = x_extent;
                }
                else
                {
                    closest.x = -x_extent;
                }


            }
            else if (Mathf.Abs(N.y) > Mathf.Abs(N.x) && Mathf.Abs(N.y) > Mathf.Abs(N.z))
            {
                if (closest.y > 0)
                {
                    closest.y = y_extent;
                }
                else
                {
                    closest.y = -y_extent;
                }

            }
            else
            {
                if (closest.z > 0)
                {
                    closest.z = z_extent;
                }
                else
                {
                    closest.z = -z_extent;
                }
            }
        }


        Vector3 localnormal = N - closest;
        float D = localnormal.sqrMagnitude;
        float R = Sphere.Radius;

        if (D > R * R && !inside)
        {
            return false;
        }

        D = Mathf.Sqrt(D);

        if (inside)
        {
            normal_point_of_contact = -N;
            penetration = R - D;
        }
        else
        {
            normal_point_of_contact = N;
            penetration = R - D;

        }

        return true;
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

    public void Plane_Friction(My_Rigid_Body A)
    {
        A.Velocity = -A.coeficient_dynamic_friction * A.Velocity;
        Impulse_Correction_for_one(A);     
    }

    public void Impulse_Correction_for_one(My_Rigid_Body A)
    {
        float Percent = 0.3f;
        float Threshhold = 0.03f;

        Vector3 correction = Mathf.Max(penetration - Threshhold, 0.0f) / (1.0f / A.mass) * Percent * normal_point_of_contact;

        A.Correction_Position += (1.0f / A.mass) * correction;


    }
}
