using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Objects_Manager : MonoBehaviour {

    My_Rigid_Body[] RigidBodies;

    private GameObject Plane;

    public float sleep_threshold = 0.05f;

    void Start ()
    {
        Plane = GameObject.FindGameObjectWithTag("Plane");	
	}

	void Update ()
    {
		
	}

    public void Object_Movement()
    {
        RigidBodies = (My_Rigid_Body[])GameObject.FindObjectsOfType(typeof(My_Rigid_Body));

        foreach (My_Rigid_Body rigidbody in RigidBodies)
        {
            if (rigidbody.gameObject.tag != "Plane" && rigidbody.gameObject.tag != "Wall" && rigidbody.asleep_ == false)
            {
                Update_Velocity(rigidbody);

                Semi_Implicit_Euler(rigidbody);

                Reset_Force(rigidbody);

                rigidbody.gameObject.GetComponent<Renderer>().material.color = Color.green;

                float range = 0.01f;

                if ( ( (rigidbody.Velocity.x <= range && rigidbody.Velocity.x >= -range) || ( rigidbody.Velocity.z <= range && rigidbody.Velocity.z >= -range) ) && rigidbody.midair == false)
                {
                   rigidbody.asleep_ = true;
                   rigidbody.gameObject.GetComponent<Renderer>().material.color = Color.magenta;
                }
            }
        }
    }

    private void Reset_Force(My_Rigid_Body rigidbody)
    {
        rigidbody.Force = Vector3.zero;
    }

    private void Semi_Implicit_Euler(My_Rigid_Body rigidbody)
    {
        rigidbody.transform.position = rigidbody.Correction_Position + (rigidbody.Velocity * Time.fixedDeltaTime);
        rigidbody.Correction_Position = rigidbody.transform.position;
    }

    private void Update_Velocity(My_Rigid_Body rigidbody)
    {
        rigidbody.Force = new Vector3(0.0f, rigidbody.gravity, 0.0f);

        rigidbody.Velocity += rigidbody.Force * Time.fixedDeltaTime;
    }
}
