/** \file BGPTorus.h
 *  Author: Abhinav S Bhatele
 *  Date created: May 21st, 2007  
 *  
 */

#ifndef _BGP_TORUS_H_
#define _BGP_TORUS_H_

#include "converse.h"

#if CMK_BLUEGENEP

#include "dcmf.h"

class BGPTorusManager {
  private:
    DCMF_Hardware_t bgp_hwt;
    int dimX;	// dimension of the allocation in X (no. of processors)
    int dimY;	// dimension of the allocation in Y (no. of processors)
    int dimZ;	// dimension of the allocation in Z (no. of processors)
    int dimNX;	// dimension of the allocation in X (no. of nodes)
    int dimNY;	// dimension of the allocation in Y (no. of nodes)
    int dimNZ;	// dimension of the allocation in Z (no. of nodes)
    int dimNT;  // dimension of the allocation in T (no. of processors per node)
   
    int torus[4];
    int procsPerNode;
    char *mapping;

  public:
    BGPTorusManager() {
      DCMF_Hardware(&bgp_hwt);
      dimNX = bgp_hwt.xSize;
      dimNY = bgp_hwt.ySize;
      dimNZ = bgp_hwt.zSize;
      dimNT = bgp_hwt.tSize;
 
      dimX = dimNX;
      dimY = dimNY;
      dimZ = dimNZ;
 
      if(bgp_hwt.tSize != 1) {
        dimX = dimX * bgp_hwt.tSize;
      }
      procsPerNode = bgp_hwt.tSize;

      torus[0] = bgp_hwt.xTorus;
      torus[1] = bgp_hwt.yTorus;
      torus[2] = bgp_hwt.zTorus;
      torus[3] = bgp_hwt.tTorus;
      
      mapping = (char *)malloc(sizeof(char)*4);
      mapping = getenv("BG_MAPPING");
    }

    ~BGPTorusManager() { }

    inline int getDimX() { return dimX; }
    inline int getDimY() { return dimY; }
    inline int getDimZ() { return dimZ; }

    inline int getDimNX() { return dimNX; }
    inline int getDimNY() { return dimNY; }
    inline int getDimNZ() { return dimNZ; }
    inline int getDimNT() { return dimNT; }

    inline int getProcsPerNode() { return procsPerNode; }

    inline int* isTorus() { return torus; }

    inline void rankToCoordinates(int pe, int &x, int &y, int &z) {
      x = pe % dimX;
      y = (pe % (dimX*dimY)) / dimX;
      z = pe / (dimX*dimY);
    }

    inline void rankToCoordinates(int pe, int &x, int &y, int &z, int &t) {
      if(strcmp(mapping, "XYZT")) {
        x = pe % dimNX;
        y = (pe % (dimNX*dimNY)) / dimNX;
        z = (pe % (dimNX*dimNY*dimNZ)) / (dimNX*dimNY);
        t = pe / (dimNX*dimNY*dimNZ);
      } else {
        t = pe % dimNT;
        x = (pe % (dimNT*dimNX)) / dimNT;
        y = (pe % (dimNT*dimNX*dimNY)) / (dimNT*dimNX);
        z = pe / (dimNT*dimNX*dimNY);
      }
    }

    inline int coordinatesToRank(int x, int y, int z) {
      return x + y*dimX + z*dimX*dimY;
    }

    inline int coordinatesToRank(int x, int y, int z, int t) {
      if(strcmp(mapping, "XYZT"))
        return x + y*dimNX + z*dimNX*dimNY + t*dimNX*dimNY*dimNZ;
      else
        return t + x*dimNT + y*dimNT*dimNX + z*dimNT*dimNX*dimNY;
    }
};

#endif // CMK_BLUEGENEP
#endif //_BGP_TORUS_H_
