using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/*
 * Each object that needs to move will have to make us of this class.
 * This is where the target position is given and the request is then sent to the manager's queue.
 * After the algorithm produce a path for this request, it returns as a vector of 3D positions which can be followed.
 */ 

public class Pathfinding_Unit : MonoBehaviour
{
    public Transform target;
    public Vector3 startPosition;
    public float pathUpdateDistance;
    public float stoppingDistance;

    public float speed = 20;
    public float timeBetweenUpdate;
    public float updateTimer;
    Vector3[] path;
    int targetIndex;

    public bool move;

    void Start()
    {
        PathRequestManager.RequestPath(transform.position, target.position, OnPathFound);
    }

    void Update()
    {
        CheckTarget();
    }

    void CheckTarget()
    {
        float distance = Vector3.Distance(target.position,startPosition);

        if(distance > pathUpdateDistance)
        {
            PathRequestManager.RequestPath(transform.position, target.position, OnPathFound);
        }
    }

    public void OnPathFound(Vector3[] newPath, bool pathSuccessful)
    {
        if (pathSuccessful)
        {
            path = newPath;
            //targetIndex = 0;
            StopCoroutine("FollowPath");
            StartCoroutine("FollowPath");

            if(target != null)
            {
                startPosition = target.position;
            }
        }
    }

    IEnumerator FollowPath()
    {
        Vector3 currentWaypoint = Vector3.zero;

        if (path.Length != 0)
        {
            currentWaypoint = path[0];
        }
         

        while (true)
        {
            if (transform.position == currentWaypoint)
            {
                targetIndex++;

                if (targetIndex >= path.Length)
                {
                    yield break;
                }

                currentWaypoint = path[targetIndex];
            }

            if(path.Length != 0)
            {
                if (move)
                {
                    float distance = Vector3.Distance(target.position, startPosition);
                    if (distance > stoppingDistance)
                    {
                        transform.position = Vector3.MoveTowards(transform.position, currentWaypoint, speed * Time.deltaTime);
                    }          
                }           
            }            
            yield return null;
        }

    }

    public void OnDrawGizmos()
    {
        if (path != null)
        {
            for (int i = targetIndex; i < path.Length; i++)
            {
                Gizmos.color = Color.black;
                Gizmos.DrawCube(path[i], Vector3.one);

                if (i == targetIndex)
                {
                    Gizmos.DrawLine(transform.position, path[i]);
                }
                else
                {
                    Gizmos.DrawLine(path[i - 1], path[i]);
                }
            }
        }
    }
}
