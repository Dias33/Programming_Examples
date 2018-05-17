using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;

/*
 * Heap is a data structure built for this scenario with the intent on increasing the performance of the pathfinding algorithm.
 * When looking through the neighbours of a node in the A* algorithm,
 * it is far more advantageous to use this data structure as it registers all nodes in a parent/child relantionship based on their f-cost.
 * 
 * The rules are:
 * Each node has only 1 parent node and a maximum of 2 child nodes.
 * The root node is the node with the lowest f-cost.
 * Each child node must have its f-cost lower than its parent.
 * 
 * Thus, the following formulas can be applied to find a specific node faster:
 * Parent Node = (n-1) / 2;
 * "Left" Child = 2n + 1;
 * "Right" Child = 2n + 2;
 * where "n" is the heap index of each member of the Heap data structure. 
 */

public class Heap<T> where T : IHeapItem<T>
{
    T[] items;
    int currentItemCount;

    public Heap(int maxHeapSize)
    {
        items = new T[maxHeapSize];
    }

   /*
    * Add a node to the Heap data structure.
    */

    public void Add(T item)
    {
        item.HeapIndex = currentItemCount;
        items[currentItemCount] = item;
        SortUp(item);
        currentItemCount++;
    }

    /*
     * Remove the first node of the Heap data structure.
     */
    public T RemoveFirst()
    {
        T firstitem = items[0];
        currentItemCount--;
        items[0] = items[currentItemCount];
        items[0].HeapIndex = 0;
        SortDown(items[0]);
        return firstitem;
    }

    public int Count
    {
        get
        {
            return currentItemCount;
        }
    }

    public void UpdateItem(T item)
    {
        SortUp(item);
    }

    public bool Contains(T item)
    {
        return Equals(items[item.HeapIndex], item);
    }

    /*
    * Puts the given item in the Heap data structure in the right place according to the rules. 
    * Iterates from the given item "down" through its children.
    */

    void SortDown(T item)
    {
        while (true)
        {
            int childIndexLeft = item.HeapIndex * 2 + 1;
            int childIndexRight = item.HeapIndex * 2 + 2;
            int swapIndex = 0;

            if (childIndexLeft < currentItemCount)
            {
                swapIndex = childIndexLeft;

                if (childIndexRight < currentItemCount)
                {
                    if (items[childIndexLeft].CompareTo(items[childIndexRight]) < 0)
                    {
                        swapIndex = childIndexRight;
                    }
                }
                if (item.CompareTo(items[swapIndex]) < 0)
                {
                    Swap(item, items[swapIndex]);
                }
                else
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }
    }

    /*
    * Puts the given item in the Heap data structure in the right place according to the rules.
    * Iterates from the given item "up" through its parents.
    */

    void SortUp(T item)
    {
        int parentIndex = (item.HeapIndex - 1) / 2;

        while (true)
        {
            T parentItem = items[parentIndex];

            if (item.CompareTo(parentItem) > 0)
            {
                Swap(item, parentItem);
            }
            else
            {
                break;
            }

            parentIndex = (item.HeapIndex - 1) / 2;
        }
    }

    /*
     * Swap the places of two items in the Heap data structure.
     */
    void Swap(T itemA, T itemB)
    {
        items[itemA.HeapIndex] = itemB;
        items[itemB.HeapIndex] = itemA;

        int temp = itemA.HeapIndex;
        itemA.HeapIndex = itemB.HeapIndex;
        itemB.HeapIndex = temp;
    }
}

    /*
     * Makes the generic type of items "T" to be able to keep track of their index.
     * Makes the generic type of items "T" comparable between themselves.
     */

    public interface IHeapItem<T> : IComparable<T>
    {
        int HeapIndex
        {
            get;
            set;
        }
    }
