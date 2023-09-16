#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;
#define outputFileName "output_file.txt"

struct Node {
    int rideNumber;
    int rideCost;
    int tripDuration;
    Node * parentNode;
    Node * leftChildNode;
    Node * rightChildNode;
    int nodeColor;
    Node * linkNodePtr;
    int index;
};
typedef Node * TreeNodePtr;

class gatorTaxi {
    private: TreeNodePtr root;
    TreeNodePtr NULLNODE;
    ofstream outputFile;
    vector < TreeNodePtr > minHeap;

    // Returns the index of the right child of a node in a min-heap
    int minHeapRightChild(int nodeIndex) {
        return 2 * nodeIndex + 2;
    }

    // Returns the index of the left child of a node in a min-heap
    int minHeapLeftChild(int nodeIndex) {
        return 2 * nodeIndex + 1;
    }

    // Returns the index of the parent of a node in a min-heap
    int minHeapParent(int nodeIndex) {
        return (nodeIndex - 1) / 2;
    }

    // Swaps two nodes in a min-heap array and updates their indices
    void swapMinHeapNodes(int index1, int index2) {
        // Store node at index1 in temp variable
        TreeNodePtr tempNode = minHeap[index1];
        // Copy node at index2 to index1
        minHeap[index1] = minHeap[index2];
        // Copy temp node to index2
        minHeap[index2] = tempNode;
        // Update index values for swapped nodes
        int indexSwap = minHeap[index1] -> index;
        minHeap[index1] -> index = minHeap[index2] -> index;
        minHeap[index2] -> index = indexSwap;
    }

    // Rebalances the heap by swapping a node with its parent until the heap property is restored
    void heapifyUp(int nodeIndex) {
        // Check if the node index is valid and if the parent node has a greater or equal ride cost
        if (nodeIndex && minHeap[minHeapParent(nodeIndex)] -> rideCost >= minHeap[nodeIndex] -> rideCost) {
            // If the parent node has the same ride cost as the current node, compare trip durations
            if (minHeap[minHeapParent(nodeIndex)] -> rideCost == minHeap[nodeIndex] -> rideCost) {
                if (minHeap[minHeapParent(nodeIndex)] -> tripDuration > minHeap[nodeIndex] -> tripDuration) {
                    // Swap the nodes and recursively call heapifyUp on the parent node
                    swapMinHeapNodes(minHeapParent(nodeIndex), nodeIndex);
                    heapifyUp(minHeapParent(nodeIndex));
                }
            } else {
                // Swap the nodes and recursively call heapifyUp on the parent node
                swapMinHeapNodes(minHeapParent(nodeIndex), nodeIndex);
                heapifyUp(minHeapParent(nodeIndex));
            }
        }
    }

    // Rebalances the heap by swapping a node with its smallest child until the heap property is restored
    void heapifyDown(int nodeIndex) {
        // Get indices of left and right child nodes
        int leftChild = minHeapLeftChild(nodeIndex);
        int rightChild = minHeapRightChild(nodeIndex);
        // Initialize the smallest node as the current node
        int smallestNode = nodeIndex;

        // Compare the ride cost of the left child node with the current smallest node
        if (leftChild < minHeap.size() && minHeap[leftChild] -> rideCost < minHeap[smallestNode] -> rideCost) {
            smallestNode = leftChild;
        } else if (leftChild < minHeap.size() && minHeap[leftChild] -> rideCost == minHeap[smallestNode] -> rideCost) {
            // If the ride cost of the left child is the same as the current smallest node, compare trip durations
            if (minHeap[leftChild] -> tripDuration < minHeap[smallestNode] -> tripDuration) {
                smallestNode = leftChild;
            }
        }

        // Compare the ride cost of the right child node with the current smallest node
        if (rightChild < minHeap.size() && minHeap[rightChild] -> rideCost < minHeap[smallestNode] -> rideCost) {
            smallestNode = rightChild;
        } else if (rightChild < minHeap.size() && minHeap[rightChild] -> rideCost == minHeap[smallestNode] -> rideCost) {
            // If the ride cost of the right child is the same as the current smallest node, compare trip durations
            if (minHeap[rightChild] -> tripDuration < minHeap[smallestNode] -> tripDuration) {
                smallestNode = rightChild;
            }
        }

        // If the smallest node is not the current node, swap the nodes and recursively call heapifyDown on the smallest node
        if (smallestNode != nodeIndex) {
            swapMinHeapNodes(nodeIndex, smallestNode);
            heapifyDown(smallestNode);
        }
    }

    // This function searches for a node in a Red-Black Tree based on its rideNumber
    // The function takes in a Red-Black Tree node pointer and the rideNumber to search for
    TreeNodePtr searchRBTreeHelper(TreeNodePtr rbNode, int rideNumber) {

        // If the node pointer is NULL or the rideNumber of the node matches the search rideNumber, return the node
        if (rbNode == NULLNODE || rideNumber == rbNode -> rideNumber) return rbNode;

        // If the search rideNumber is greater than or equal to the rideNumber of the current node,
        // search in the right subtree of the current node
        if (rideNumber >= rbNode -> rideNumber) return searchRBTreeHelper(rbNode -> rightChildNode, rideNumber);

        // If the search rideNumber is less than the rideNumber of the current node,
        // search in the left subtree of the current node
        return searchRBTreeHelper(rbNode -> leftChildNode, rideNumber);
    }

    void deleteRBNodeFix(TreeNodePtr nodeX) {
        TreeNodePtr nodeS;
        while (nodeX != root && nodeX -> nodeColor == 0) {
            // If the node being deleted is a left child
            if (nodeX == nodeX -> parentNode -> leftChildNode) {
                // Get the sibling of the node being deleted
                nodeS = nodeX -> parentNode -> rightChildNode;
                // If the sibling is red, recolor and perform a left rotation on parent
                if (nodeS -> nodeColor == 1) {
                    nodeS -> nodeColor = 0;
                    nodeX -> parentNode -> nodeColor = 1;
                    rbTreeLeftRotate(nodeX -> parentNode);
                    nodeS = nodeX -> parentNode -> rightChildNode;
                }
                // If both children of the sibling are black, recolor and move up to the parent
                if (nodeS -> leftChildNode -> nodeColor == 0 && nodeS -> rightChildNode -> nodeColor == 0) {
                    nodeS -> nodeColor = 1;
                    nodeX = nodeX -> parentNode;
                } else {
                    // If the sibling's right child is black, recolor and perform a right rotation on the sibling
                    if (nodeS -> rightChildNode -> nodeColor == 0) {
                        nodeS -> leftChildNode -> nodeColor = 0;
                        nodeS -> nodeColor = 1;
                        rbTreeRightRotate(nodeS);
                        nodeS = nodeX -> parentNode -> rightChildNode;
                    }
                    // Recolor the sibling and parent, and recolor the sibling's right child black
                    nodeS -> nodeColor = nodeX -> parentNode -> nodeColor;
                    nodeX -> parentNode -> nodeColor = 0;
                    nodeS -> rightChildNode -> nodeColor = 0;
                    // Perform a left rotation on the parent and set the nodeX to the root
                    rbTreeLeftRotate(nodeX -> parentNode);
                    nodeX = root;
                }
            } else {
                // If the node being deleted is a right child
                // Same operations as above with "left" and "right" swapped
                nodeS = nodeX -> parentNode -> leftChildNode;
                if (nodeS -> nodeColor == 1) {
                    nodeS -> nodeColor = 0;
                    nodeX -> parentNode -> nodeColor = 1;
                    rbTreeRightRotate(nodeX -> parentNode);
                    nodeS = nodeX -> parentNode -> leftChildNode;
                }
                if (nodeS -> rightChildNode -> nodeColor == 0 && nodeS -> rightChildNode -> nodeColor == 0) {
                    nodeS -> nodeColor = 1;
                    nodeX = nodeX -> parentNode;
                } else {
                    if (nodeS -> leftChildNode -> nodeColor == 0) {
                        nodeS -> rightChildNode -> nodeColor = 0;
                        nodeS -> nodeColor = 1;
                        rbTreeLeftRotate(nodeS);
                        nodeS = nodeX -> parentNode -> leftChildNode;
                    }
                    nodeS -> nodeColor = nodeX -> parentNode -> nodeColor;
                    nodeX -> parentNode -> nodeColor = 0;
                    nodeS -> leftChildNode -> nodeColor = 0;
                    rbTreeRightRotate(nodeX -> parentNode);
                    nodeX = root;
                }
            }
        }
        // Set the nodeX (originally the deleted node) to black to maintain the red-black tree properties
        nodeX -> nodeColor = 0;
    }

    // This function replaces the subtree rooted at nodeU with the subtree rooted at nodeV
    void rbTreeNodeTransplant(TreeNodePtr nodeU, TreeNodePtr nodeV) {
        if (nodeU -> parentNode == nullptr) { // If nodeU is the root node
            root = nodeV; // Make nodeV the new root
        } else if (nodeU == nodeU -> parentNode -> leftChildNode) { // If nodeU is a left child of its parent
            nodeU -> parentNode -> leftChildNode = nodeV; // Replace nodeU with nodeV as the left child of its parent
        } else { // If nodeU is a right child of its parent
            nodeU -> parentNode -> rightChildNode = nodeV; // Replace nodeU with nodeV as the right child of its parent
        }
        nodeV -> parentNode = nodeU -> parentNode; // Set the parent of nodeV to be the parent of nodeU
    }

    void deleteRBTreeNodeHelper(TreeNodePtr rbTreeNode, int rideNumber) {
        // Find the node to be deleted
        TreeNodePtr nodeZ = NULLNODE;
        TreeNodePtr nodeX, nodeY;
        while (rbTreeNode != NULLNODE) {
            if (rbTreeNode -> rideNumber == rideNumber) {
                nodeZ = rbTreeNode;
            }

            if (rbTreeNode -> rideNumber <= rideNumber) {
                rbTreeNode = rbTreeNode -> rightChildNode;
            } else {
                rbTreeNode = rbTreeNode -> leftChildNode;
            }
        }

        // If the node to be deleted is not found, return
        if (nodeZ == NULLNODE) {
            return;
        }

        // Save the node that will replace the deleted node (nodeX) and the node to splice out (nodeY)
        nodeY = nodeZ;
        int nodeYOriginalColor = nodeY -> nodeColor;
        if (nodeZ -> leftChildNode == NULLNODE) {
            nodeX = nodeZ -> rightChildNode;
            rbTreeNodeTransplant(nodeZ, nodeZ -> rightChildNode);
        } else if (nodeZ -> rightChildNode == NULLNODE) {
            nodeX = nodeZ -> leftChildNode;
            rbTreeNodeTransplant(nodeZ, nodeZ -> leftChildNode);
        } else {
            nodeY = minimumRBTreeNode(nodeZ -> rightChildNode);
            nodeYOriginalColor = nodeY -> nodeColor;
            nodeX = nodeY -> rightChildNode;
            if (nodeY -> parentNode == nodeZ) {
                nodeX -> parentNode = nodeY;
            } else {
                rbTreeNodeTransplant(nodeY, nodeY -> rightChildNode);
                nodeY -> rightChildNode = nodeZ -> rightChildNode;
                nodeY -> rightChildNode -> parentNode = nodeY;
            }

            rbTreeNodeTransplant(nodeZ, nodeY);
            nodeY -> leftChildNode = nodeZ -> leftChildNode;
            nodeY -> leftChildNode -> parentNode = nodeY;
            nodeY -> nodeColor = nodeZ -> nodeColor;
        }

        // If the deleted node has a corresponding link node, delete the link node
        if (nodeZ -> linkNodePtr -> rideNumber == nodeZ -> rideNumber && nodeZ -> linkNodePtr -> index != -1) {
            deleteMHNode(nodeZ -> linkNodePtr -> index);
        }

        // Delete the node to be deleted (nodeZ)
        delete nodeZ;

        // If the original color of nodeY is black, fix the RB tree
        if (nodeYOriginalColor == 0) {
            deleteRBNodeFix(nodeX);
        }
    }

    // Fix any violations of the Red-Black Tree properties that may have occurred after inserting nodeK
    void insertRBTreeNodeFix(TreeNodePtr nodeK) {
        TreeNodePtr nodeU;

        // Continue fixing violations until there are no more violations or the root is reached
        while (nodeK -> parentNode -> nodeColor == 1) {
            // Case 1: The parent of nodeK is a right child
            if (nodeK -> parentNode == nodeK -> parentNode -> parentNode -> rightChildNode) {
                // Set nodeU to the left child of the grandparent of nodeK
                nodeU = nodeK -> parentNode -> parentNode -> leftChildNode;

                // Case 1.1: nodeU is red
                if (nodeU -> nodeColor == 1) {
                    // Set the colors of nodeU, its parent, and its grandparent
                    nodeU -> nodeColor = 0;
                    nodeK -> parentNode -> nodeColor = 0;
                    nodeK -> parentNode -> parentNode -> nodeColor = 1;

                    // Continue fixing violations with the grandparent of nodeK
                    nodeK = nodeK -> parentNode -> parentNode;
                }
                // Case 1.2: nodeU is black
                else {
                    // Case 1.2.1: nodeK is a left child
                    if (nodeK == nodeK -> parentNode -> leftChildNode) {
                        // Continue fixing violations with the parent of nodeK
                        nodeK = nodeK -> parentNode;
                        // Rotate right around the parent of nodeK
                        rbTreeRightRotate(nodeK);
                    }
                    // Case 1.2.2: nodeK is a right child
                    // Set the colors of nodeK's parent and grandparent
                    nodeK -> parentNode -> nodeColor = 0;
                    nodeK -> parentNode -> parentNode -> nodeColor = 1;

                    // Rotate left around the grandparent of nodeK
                    rbTreeLeftRotate(nodeK -> parentNode -> parentNode);
                }
            }
            // Case 2: The parent of nodeK is a left child
            else {
                // Set nodeU to the right child of the grandparent of nodeK
                nodeU = nodeK -> parentNode -> parentNode -> rightChildNode;

                // Case 2.1: nodeU is red
                if (nodeU -> nodeColor == 1) {
                    // Set the colors of nodeU, its parent, and its grandparent
                    nodeU -> nodeColor = 0;
                    nodeK -> parentNode -> nodeColor = 0;
                    nodeK -> parentNode -> parentNode -> nodeColor = 1;

                    // Continue fixing violations with the grandparent of nodeK
                    nodeK = nodeK -> parentNode -> parentNode;
                }
                // Case 2.2: nodeU is black
                else {
                    // Case 2.2.1: nodeK is a right child
                    if (nodeK == nodeK -> parentNode -> rightChildNode) {
                        // Continue fixing violations with the parent of nodeK
                        nodeK = nodeK -> parentNode;
                        // Rotate left around the parent of nodeK
                        rbTreeLeftRotate(nodeK);
                    }
                    // Case 2.2.2: nodeK is a left child
                    // Set the colors of nodeK's parent and grandparent
                    nodeK -> parentNode -> nodeColor = 0;
                    nodeK -> parentNode -> parentNode -> nodeColor = 1;

                    // Rotate right around the grandparent of nodeK
                    rbTreeRightRotate(nodeK -> parentNode -> parentNode);
                }
            }
            if (nodeK == root) {
                break;
            }
        }
        root -> nodeColor = 0;
    }

    public:

        void insertMHTree(TreeNodePtr nodeR) {
            // Set the index of the new node to the current size of the min heap vector
            nodeR -> index = minHeap.size();

            // Add the new node to the end of the min heap vector
            minHeap.push_back(nodeR);

            // Get the index of the new node in the min heap vector
            int heapIndex = minHeap.size() - 1;

            // Perform heapify-up operation to maintain the min-heap property
            heapifyUp(heapIndex);
        }

    TreeNodePtr getTop() {
        // If the min heap is empty, throw an exception
        if (minHeap.size() == 0) {
            throw "Heap is empty!";
        }

        // Return the root of the min heap, which is the top element of the heap
        return minHeap[0];
    }

    void deleteMin() {
        if (minHeap.size() == 0) {
            // If the min heap is empty, return without doing anything
            return;
        }

        // Get the red-black tree node associated with the minimum heap node
        TreeNodePtr rbNodePtr = minHeap[0] -> linkNodePtr;

        // Swap the first and last elements of the min heap, and update their indices
        swapMinHeapNodes(0, minHeap.size() - 1);
        minHeap[minHeap.size() - 1] -> index = -1;
        delete minHeap[minHeap.size() - 1];
        minHeap.pop_back();

        // Restore the heap property by "bubbling down" the root node
        heapifyDown(0);

        // Delete the red-black tree node associated with the minimum heap node
        deleteRBTreeNodeHelper(rbNodePtr, rbNodePtr -> rideNumber);
    }

    TreeNodePtr getMin() {
        // Return the root node of the min heap (which has the smallest key)
        return minHeap[0];
    }

    bool empty() {
        // Check if the min heap is empty
        return minHeap.empty();
    }

    TreeNodePtr GetNextRide() {
        // If minHeap is empty, return a new Node with default values
        if (minHeap.empty()) {
            outputFile << "No active ride requests" << endl;
            TreeNodePtr nodeP = new Node;
            nodeP -> rideNumber = 0;
            nodeP -> rideCost = 0;
            nodeP -> tripDuration = 0;
            nodeP -> parentNode = nullptr;
            nodeP -> leftChildNode = NULLNODE;
            nodeP -> rightChildNode = NULLNODE;
            nodeP -> nodeColor = 0;
            nodeP -> linkNodePtr = nullptr;
            return nodeP;
        }

        // Get the next ride request from the top of the minHeap
        TreeNodePtr nextRide = getTop();

        // Output the ride details to the output file
        outputFile << "(" << nextRide -> rideNumber << "," << nextRide -> rideCost << "," << nextRide -> tripDuration << ")" << endl;

        // Delete the top node from the minHeap
        deleteMin();

        // Return the next ride request node
        return nextRide;
    }

    // This function initializes a new node and assigns default values to its members.
    // It then sets the root of the binary search tree to this node.
    // It also opens a file named "outputFileName" for writing output.
    gatorTaxi() {
        NULLNODE = new Node;
        NULLNODE -> rideNumber = 0;
        NULLNODE -> rideCost = 0;
        NULLNODE -> tripDuration = 0;
        NULLNODE -> nodeColor = 0;
        NULLNODE -> leftChildNode = nullptr;
        NULLNODE -> rightChildNode = nullptr;
        NULLNODE -> linkNodePtr = nullptr;
        NULLNODE -> parentNode = nullptr;
        NULLNODE -> index = -1;
        root = NULLNODE;
        outputFile.open(outputFileName);
    }

    TreeNodePtr searchRBTree(int rideNumber) {
        // Call the helper function searchRBTreeHelper to traverse the tree recursively.
        return searchRBTreeHelper(this -> root, rideNumber);
    }

    TreeNodePtr minimumRBTreeNode(TreeNodePtr rbTreeNode) {
        // Follow the left child pointers until we reach a node with no left child.
        while (rbTreeNode -> leftChildNode != NULLNODE) {
            rbTreeNode = rbTreeNode -> leftChildNode;
        }
        // Return the node with the minimum key.
        return rbTreeNode;
    }

    // Perform a left rotation on the red-black tree at nodeX.
    void rbTreeLeftRotate(TreeNodePtr nodeX) {
        // Store nodeY as the right child of nodeX.
        TreeNodePtr nodeY = nodeX -> rightChildNode;
        // Set the right child of nodeX to be the left child of nodeY.
        nodeX -> rightChildNode = nodeY -> leftChildNode;
        // Update the parent of the left child of nodeY to be nodeX.
        if (nodeY -> leftChildNode != NULLNODE) {
            nodeY -> leftChildNode -> parentNode = nodeX;
        }
        // Update the parent of nodeY to be the parent of nodeX.
        nodeY -> parentNode = nodeX -> parentNode;
        // If nodeX is the root of the tree, set nodeY as the new root.
        if (nodeX -> parentNode == nullptr) {
            this -> root = nodeY;
        }
        // If nodeX is the left child of its parent, set nodeY as the left child of the parent.
        else if (nodeX == nodeX -> parentNode -> leftChildNode) {
            nodeX -> parentNode -> leftChildNode = nodeY;
        }
        // Otherwise, set nodeY as the right child of the parent.
        else {
            nodeX -> parentNode -> rightChildNode = nodeY;
        }
        // Set nodeX as the left child of nodeY and nodeY as the parent of nodeX.
        nodeY -> leftChildNode = nodeX;
        nodeX -> parentNode = nodeY;
    }

    // Perform a right rotation on the given node in the red-black tree.
    // Parameters:
    // nodeX: The node to rotate.
    void rbTreeRightRotate(TreeNodePtr nodeX) {
        // Set nodeY to the left child of nodeX.
        TreeNodePtr nodeY = nodeX -> leftChildNode;
        // Set the left child of nodeX to be the right child of nodeY.
        nodeX -> leftChildNode = nodeY -> rightChildNode;
        if (nodeY -> rightChildNode != NULLNODE) {
            // Set the parent of the right child of nodeY to be nodeX, if it exists.
            nodeY -> rightChildNode -> parentNode = nodeX;
        }
        // Set the parent of nodeY to be the parent of nodeX.
        nodeY -> parentNode = nodeX -> parentNode;
        if (nodeX -> parentNode == nullptr) {
            // If nodeX is the root of the tree, set the root to be nodeY.
            this -> root = nodeY;
        } else if (nodeX == nodeX -> parentNode -> rightChildNode) {
            // If nodeX is the right child of its parent, set the right child of the parent to be nodeY.
            nodeX -> parentNode -> rightChildNode = nodeY;
        } else {
            // Otherwise, set the left child of the parent to be nodeY.
            nodeX -> parentNode -> leftChildNode = nodeY;
        }
        // Set the right child of nodeY to be nodeX, and set the parent of nodeX to be nodeY.
        nodeY -> rightChildNode = nodeX;
        nodeX -> parentNode = nodeY;
    }

    void insertRBTree(TreeNodePtr rbTreeNode) {
        // Initialize nodeY as nullptr and nodeX as the root of the tree.
        TreeNodePtr nodeY = nullptr;
        TreeNodePtr nodeX = this -> root;

        // Traverse the tree until we find the proper place to insert the new node.
        while (nodeX != NULLNODE) {
            nodeY = nodeX;
            // If the ride number of the new node is equal to the ride number of the current node, exit the program as duplicates are not allowed.
            if (rbTreeNode -> rideNumber == nodeX -> rideNumber) {
                outputFile << "Duplicate RideNumber" << endl;
                exit(0);
            } else if (rbTreeNode -> rideNumber < nodeX -> rideNumber) {
                // If the ride number of the new node is less than the ride number of the current node, move to the left child of the current node.
                nodeX = nodeX -> leftChildNode;
            } else {
                // If the ride number of the new node is greater than the ride number of the current node, move to the right child of the current node.
                nodeX = nodeX -> rightChildNode;
            }
        }

        // Set the parent of the new node to nodeY.
        rbTreeNode -> parentNode = nodeY;
        if (nodeY == nullptr) {
            // If nodeY is nullptr, set the root of the tree to the new node.
            root = rbTreeNode;
        } else if (rbTreeNode -> rideNumber < nodeY -> rideNumber) {
            // If the ride number of the new node is less than the ride number of nodeY, set the left child of nodeY to the new node.
            nodeY -> leftChildNode = rbTreeNode;
        } else {
            // If the ride number of the new node is greater than or equal to the ride number of nodeY, set the right child of nodeY to the new node.
            nodeY -> rightChildNode = rbTreeNode;
        }

        // If the parent of the new node is nullptr, set the color of the new node to black and return.
        if (rbTreeNode -> parentNode == nullptr) {
            rbTreeNode -> nodeColor = 0;
            return;
        }

        // If the grandparent of the new node is nullptr, return.
        if (rbTreeNode -> parentNode -> parentNode == nullptr) return;

        // Fix the tree using the insertRBTreeNodeFix function.
        insertRBTreeNodeFix(rbTreeNode);
    }

    // Deletes the node with the given ride number from the red-black tree
    void deleteRBTreeNode(int rideNumber) {
        // Calls the helper function to delete the node starting from the root of the tree
        deleteRBTreeNodeHelper(this -> root, rideNumber);
    }

    // Creates and returns a new node with the given properties
    TreeNodePtr createNode(int rideNumber, int rideCost, int tripDuration) {
        // Allocates memory for the new node
        TreeNodePtr nodeP = new Node;

        // Initializes the properties of the new node
        nodeP -> rideNumber = rideNumber;
        nodeP -> rideCost = rideCost;
        nodeP -> tripDuration = tripDuration;
        nodeP -> parentNode = nullptr;
        nodeP -> leftChildNode = NULLNODE;
        nodeP -> rightChildNode = NULLNODE;
        nodeP -> nodeColor = 1;
        nodeP -> linkNodePtr = nullptr;
        nodeP -> index = -1;

        // Returns the new node
        return nodeP;
    }

    // Function to print nodes within a given range
    void Print(int lowerBound, int upperBound) {

        // Create a vector to store the nodes that fall within the range
        vector < string > printRangeArray;

        // Call the recursive function to find the nodes within the range and add them to the vector
        printRange(root, lowerBound, upperBound, printRangeArray);

        // Get the size of the vector
        int n = printRangeArray.size();

        // If the vector is empty, print "(0,0,0)" to the output file
        if (n == 0) {
            outputFile << "(0,0,0)";
        } else {
            // Otherwise, loop through the vector and print each node to the output file separated by commas
            for (int i = 0; i < n - 1; i++) {
                outputFile << printRangeArray[i] << ",";
            }
            // Print the last node without a comma after it
            outputFile << printRangeArray[n - 1];
        }

        // Print a newline character to the output file
        outputFile << endl;
    }

    // Recursive function to print rides with rideNumber between lowerBound and upperBound
    // Takes in a TreeNodePtr node, which represents the current node being evaluated
    // Also takes in the lower and upper bounds for the ride numbers to be printed
    // Returns an integer representing the number of rides printed
    void printRange(TreeNodePtr node, int lowerBound, int upperBound, vector < string > & printRangeArray) {
        if (node == nullptr) return;
        // If rideNumber is greater than the upperBound, move to the left subtree
        if (node -> rideNumber > upperBound) {
            printRange(node -> leftChildNode, lowerBound, upperBound, printRangeArray);
        }
        // If rideNumber is less than the lowerBound, move to the right subtree
        else if (node -> rideNumber < lowerBound) {
            printRange(node -> rightChildNode, lowerBound, upperBound, printRangeArray);
        }
        // If rideNumber is within the bounds, recursively search left and right subtrees
        // Print ride and increment count if ride is within bounds
        else {
            printRange(node -> leftChildNode, lowerBound, upperBound, printRangeArray);
            if (node -> rideNumber >= lowerBound && node -> rideNumber <= upperBound) {
                printRangeArray.push_back("(" + to_string(node -> rideNumber) + "," + to_string(node -> rideCost) + "," + to_string(node -> tripDuration) + ")");
            }
            printRange(node -> rightChildNode, lowerBound, upperBound, printRangeArray);
        }
    }

    // Removes a ride with the given rideNumber from the Red-Black Tree
    void CancelRide(int rideNumber) {
        deleteRBTreeNode(rideNumber);
    }

    // Deletes a node from the Min-Heap with the given index
    void deleteMHNode(int index) {
        // Swap the node with the last node in the Min-Heap
        swapMinHeapNodes(minHeap[minHeap.size() - 1] -> index, index);
        // Set the index of the last node to -1 to mark it as deleted
        minHeap[minHeap.size() - 1] -> index = -1;
        // Delete the last node from memory and remove it from the Min-Heap vector
        delete minHeap[minHeap.size() - 1];
        minHeap.pop_back();
        // Reorder the Min-Heap from the given index downwards
        heapifyDown(index);
    }

    void UpdateTrip(int rideNumber, int newTripDuration) {
        // search the rideNumber in the Red-Black Tree
        TreeNodePtr node = searchRBTree(rideNumber);

        // get the existing trip duration and ride cost of the ride
        int existingTripDuration = node -> tripDuration;
        int existingRideCost = node -> rideCost;

        // check if new trip duration is less than or equal to the existing trip duration
        if (newTripDuration <= existingTripDuration) {
            // update the trip duration of the ride in Red-Black Tree
            node -> tripDuration = newTripDuration;
            // update the trip duration of the ride in Min Heap
            node -> linkNodePtr -> tripDuration = newTripDuration;
            // restore the Min Heap property by moving the node up
            heapifyUp(node -> linkNodePtr -> index);
        }

        // check if new trip duration is greater than the existing trip duration but less than or equal to twice the existing trip duration
        else if (existingTripDuration < newTripDuration && newTripDuration <= 2 * existingTripDuration) {
            // cancel the existing ride
            CancelRide(rideNumber);
            // insert a new ride with the updated trip duration and increased ride cost by $10
            Insert(rideNumber, existingRideCost + 10, newTripDuration);
        }

        // if new trip duration is more than twice the existing trip duration
        else if (newTripDuration > 2 * existingTripDuration) {
            // cancel the ride
            CancelRide(rideNumber);
        }
    }

    void Insert(int rideNumber, int rideCost, int tripDuration) {
        // create RBNode and MHNode with the same values
        TreeNodePtr RBNode = createNode(rideNumber, rideCost, tripDuration);
        TreeNodePtr MHNode = createNode(rideNumber, rideCost, tripDuration);

        // link the two nodes to each other
        RBNode -> linkNodePtr = MHNode;
        MHNode -> linkNodePtr = RBNode;

        // insert the nodes into the RBTree and Min Heap Tree
        insertRBTree(RBNode);
        insertMHTree(MHNode);
    }

    void Print(int rideNumber) {
        // search for the given rideNumber in the RBTree
        TreeNodePtr rbNode = searchRBTree(rideNumber);

        // output the rideNumber, rideCost, and tripDuration to the output file
        outputFile << "(" << rbNode -> rideNumber << "," << rbNode -> rideCost << "," << rbNode -> tripDuration << ")" << endl;
    }

    void controlGatorTaxi(string inputFileName) {
        // Open the input file and read it line by line
        ifstream inputFile(inputFileName);
        string eachLine;
        while (getline(inputFile, eachLine)) {
            // Use string stream to extract relevant information from each line
            stringstream ss(eachLine);
            // If the line contains "Insert", extract the ride information and call Insert function
            if (eachLine.find("Insert") != std::string::npos) {
                int rideNumber, rideCost, tripDuration;
                char ch;
                ss.ignore(6);
                ss >> ch >> rideNumber >> ch >> rideCost >> ch >> tripDuration >> ch;
                Insert(rideNumber, rideCost, tripDuration);
            }
            // If the line contains "GetNextRide", call GetNextRide function
            else if (eachLine.find("GetNextRide") != std::string::npos) {
                GetNextRide();
            }
            // If the line contains "Print", extract the ride information and call Print function
            else if (eachLine.find("Print") != std::string::npos) {
                int rideNumber1, rideNumber2 = -1;
                char ch;
                ss.ignore(5);
                ss >> ch >> rideNumber1;
                if (ss.peek() == ',') {
                    ss.ignore(1, ',');
                    ss >> rideNumber2;
                    Print(rideNumber1, rideNumber2);
                } else {
                    Print(rideNumber1);
                }
            }
            // If the line contains "CancelRide", extract the ride information and call CancelRide function
            else if (eachLine.find("CancelRide") != std::string::npos) {
                char ch;
                int rideNumber;
                ss.ignore(10);
                ss >> ch >> rideNumber >> ch;
                CancelRide(rideNumber);
            }
            // If the line contains "UpdateTrip", extract the ride information and call UpdateTrip function
            else if (eachLine.find("UpdateTrip") != std::string::npos) {
                char ch;
                int rideNumber, newTripDuration;
                ss.ignore(10);
                ss >> ch >> rideNumber >> ch >> newTripDuration >> ch;
                UpdateTrip(rideNumber, newTripDuration);
            }
        }
        inputFile.close();
    }

};

// The main function of the program
int main(int argc, char * argv[]) {
    // Create a gatorTaxi object
    gatorTaxi gt;
    // Get the input file name from the command line arguments
    string inputFileName = argv[1];
    // Call the controlGatorTaxi method of the gatorTaxi object passing the input file name as an argument
    gt.controlGatorTaxi(inputFileName);
    // Exit the program with a success status code
    return 0;
}