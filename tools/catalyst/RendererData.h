#pragma once

#include <cassert>
#include <cmath>
#ifdef BONSAI_CATALYST_STDLIB
 #include <boost/array.hpp>
 #define bonsaistd boost
#else
 #include <array>
 #define bonsaistd std
#endif
#include <algorithm>
#include <iostream>
#include <vector>
#include "IDType.h"
#ifdef BONSAI_CATALYST_CLANG
 #include <tmmintrin.h>
#endif
#include <mpi.h>

#include <algorithm>

class RendererData
{
  public:
    typedef unsigned long long long_t;
    enum Attribute_t {
      MASS,
      VEL,
      RHO,
      H,
      NPROP};
  protected:
    const int rank, nrank;
    const MPI_Comm &comm;
    struct particle_t
    {
      float posx, posy, posz;
      IDType ID;
      float attribute[NPROP];
    };
    std::vector<particle_t> data;
    bool new_data;

    float _xmin, _ymin, _zmin, _rmin;
    float _xmax, _ymax, _zmax, _rmax;

    float _xminl, _yminl, _zminl, _rminl;
    float _xmaxl, _ymaxl, _zmaxl, _rmaxl;

    float _attributeMin[NPROP];
    float _attributeMax[NPROP];
    float _attributeMinL[NPROP];
    float _attributeMaxL[NPROP];
  
    void minmaxAttributeGlb(const Attribute_t p);
    int  getMaster() const { return 0; }
    bool isMaster() const { return getMaster() == rank; }


  public:
    RendererData(const int rank, const int nrank, const MPI_Comm &comm) : 
      rank(rank), nrank(nrank), comm(comm)
  {
    assert(rank < nrank);
    new_data = false;
  }

    void setNewData() {new_data = true;}
    void unsetNewData() { new_data = false; }
    bool isNewData() const {return new_data;}

    int n() const { return data.size(); }
    void resize(const int n) { data.resize(n); }
    ~RendererData() {}

    float  posx(const int i) const { return data[i].posx; }
    float& posx(const int i)       { return data[i].posx; }
    float  posy(const int i) const { return data[i].posy; }
    float& posy(const int i)       { return data[i].posy; }
    float  posz(const int i) const { return data[i].posz; }
    float& posz(const int i)       { return data[i].posz; }

    float  attribute(const Attribute_t p, const int i) const {return data[i].attribute[p]; }
    float& attribute(const Attribute_t p, const int i)       {return data[i].attribute[p]; }


    IDType  ID(const long_t i) const { return data[i].ID; }
    IDType& ID(const long_t i)       { return data[i].ID; }

    float xmin() const { return _xmin;} 
    float ymin() const { return _ymin;} 
    float zmin() const { return _zmin;} 
    float rmin() const { return _rmin;} 

    float xmax() const { return _xmax;} 
    float ymax() const { return _ymax;} 
    float zmax() const { return _zmax;} 
    float rmax() const { return _rmax;} 

    float attributeMin(const Attribute_t p) const { return _attributeMin[p]; }
    float attributeMax(const Attribute_t p) const { return _attributeMax[p]; }

    void setAttributeMin(const Attribute_t p, const float val) { _attributeMin[p] = val; }
    void setAttributeMax(const Attribute_t p, const float val) { _attributeMax[p] = val; }

    float xminLoc() const { return _xminl;} 
    float yminLoc() const { return _yminl;} 
    float zminLoc() const { return _zminl;} 
    float rminLoc() const { return _rminl;} 

    float xmaxLoc() const { return _xmaxl;} 
    float ymaxLoc() const { return _ymaxl;} 
    float zmaxLoc() const { return _zmaxl;} 
    float rmaxLoc() const { return _rmaxl;} 

    float attributeMinLoc(const Attribute_t p) const { return _attributeMinL[p]; }
    float attributeMaxLoc(const Attribute_t p) const { return _attributeMaxL[p]; }
    
    void randomShuffle();
    void computeMinMax();

    template<typename Func> void rescale(const Attribute_t p, const Func &scale);
    void rescaleLinear(const Attribute_t p, const float newMin, const float newMax);
    void scaleLog(const Attribute_t p, const float zeroPoint = 1.0f);
    void scaleExp(const Attribute_t p, const float zeroPoint = 1.0f);
    void clampMinMax(const Attribute_t p, const float min, const float max);

    // virtual methods

    virtual bool  isDistributed() const { return false; }
    virtual void  setNMAXSAMPLE(const int n) {};
    virtual void  distribute() {}
    virtual float getBoundBoxLow (const int i) const 
    {
      switch (i)
      {
        case  0: return xmin(); 
        case  1: return ymin();
        case  2: return zmin();
        default: return rmin();
      }
    }
    virtual float getBoundBoxHigh(const int i) const 
    {
      switch (i)
      {
        case  0: return xmax(); 
        case  1: return ymax();
        case  2: return zmax();
        default: return rmax();
      }
    }
    virtual std::vector<int> getVisibilityOrder(const bonsaistd::array<float,3> camPos) const
    {
      return std::vector<int>();
    }

};
