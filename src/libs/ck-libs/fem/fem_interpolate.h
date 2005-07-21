#ifndef __CHARM_FEM_INTERPOLATE_H
#define __CHARM_FEM_INTERPOLATE_H

#include "charm-api.h"
#include "ckvector3d.h"
#include "fem.h"
#include "fem_mesh.h"

// This is an interface for specifying solution transfer operations
// for small mesh modification operations. It provides overridable
// defaults for node and element interpolations.  These functions are passed to
// FEM_add_element and FEM_add_node operations, along with a simple argument
// struct.

class FEM_Interpolate {
  FEM_Mesh *theMesh;
 public:
  typedef struct {
    int n;
    int nodes[8];
    int dim;    
    int nNbrs;
    double coord[3];
    double frac;
  } NodalArgs;

  typedef struct {
    int e;
    int oldElement;
  } ElementArgs;

  typedef void (* FEM_InterpolateNodeFn)(NodalArgs);
  typedef void (* FEM_InterpolateElementFn)(ElementArgs);

  FEM_InterpolateNodeFn nodeEdgeFnPtr, nodeFaceFnPtr, nodeElementFnPtr;
  FEM_InterpolateElementFn elemCopyFnPtr, elemNodeFnPtr;

  /// Basic Constructor
  FEM_Interpolate() {
    nodeEdgeFnPtr = nodeFaceFnPtr = nodeElementFnPtr = NULL;
    elemCopyFnPtr = elemNodeFnPtr = NULL;
  }

  FEM_Interpolate(FEM_Mesh *m) {
    nodeEdgeFnPtr = nodeFaceFnPtr = nodeElementFnPtr = NULL;
    elemCopyFnPtr = elemNodeFnPtr = NULL;
    theMesh = m;
  }

  void FEM_InterpolateSetMesh(FEM_Mesh *m) { theMesh = m; }
  
  // Methods to set and reset interpolate functions on the fly; these will be
  // used to override defaults by Fortan code, and can also be set and reset
  // for using temporary special-purpose interpolation functions
  void FEM_SetInterpolateNodeEdgeFnPtr(FEM_InterpolateNodeFn fnPtr) {
    nodeEdgeFnPtr = fnPtr;
  }
  void FEM_SetInterpolateNodeFaceFnPtr(FEM_InterpolateNodeFn fnPtr) {
    nodeFaceFnPtr = fnPtr;
  }
  void FEM_SetInterpolateNodeElementFnPtr(FEM_InterpolateNodeFn fnPtr) {
    nodeElementFnPtr = fnPtr;
  }
  void FEM_SetInterpolateElementCopyFnPtr(FEM_InterpolateElementFn fnPtr) {
    elemCopyFnPtr = fnPtr;
  }
  void FEM_SetInterpolateElementNodeFnPtr(FEM_InterpolateElementFn fnPtr) {
    elemNodeFnPtr = fnPtr;
  }
  void FEM_ResetInterpolateNodeEdgeFnPtr() { nodeEdgeFnPtr = NULL; }
  void FEM_ResetInterpolateNodeFaceFnPtr() { nodeFaceFnPtr = NULL; }
  void FEM_ResetInterpolateNodeElementFnPtr() { nodeElementFnPtr = NULL; }
  void FEM_ResetInterpolateElementCopyFnPtr() { elemCopyFnPtr = NULL; }  
  void FEM_ResetInterpolateElementNodeFnPtr() { elemNodeFnPtr = NULL; }

  // Nodal data
  /// A node is added on an edge; interpolate from neighboring nodes
  /** A node is added on an edge; interpolate from neighboring nodes; this
      uses n, nodes[2], dim and frac. Frac is between 0.0 and 1.0 and weights 
      nodes[0]; i.e. if frac=1.0, n gets a copy of nodes[0]'s data, and
      nodes[0]'s coords **/
  virtual void FEM_InterpolateNodeOnEdge(NodalArgs args);
  /// A node is added on an face; interpolate from nodes of face
  /** A node is added on an face; interpolate from nodes of face; this uses
      n, nodes[3] or nodes[4] depending on element type, dim and coord **/
  virtual void FEM_InterpolateNodeOnFace(NodalArgs args);
  /// A node is added inside a volume; interpolate from nodes of element
  /** A node is added inside a volume; interpolate from nodes of element; this
      uses n, nodes[4] or more, and coord **/
  virtual void FEM_InterpolateNodeInElement(NodalArgs args);

  // Element data
  /// An element added is completely encapsulated by element to be removed
  /** An element added is completely encapsulated by element to be removed;
      For example, edge bisect two elements replace the old element, and we
      simply copy the data of the old element to both new elements **/
  virtual void FEM_InterpolateElementCopy(ElementArgs args);
  /// An element is added and derives data from its nodes
  /** An element is added and derives data from its nodes; assumes relevant 
      data was copied to the appropriate nodes prior to this operation; see
      the utility function below **/
  virtual void FEM_InterpolateElementFromNodes(ElementArgs args);

  // Utility
  /// Store data of an element temporarily on all nodes
  /** Store data of an element temporarily on all nodes; this data is used 
      later to derive an element's data **/
  virtual void FEM_InterpolateElementToNodes(int e);
};

#endif
