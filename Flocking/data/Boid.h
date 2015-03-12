#ifndef BOID_H
#define BOID_H

#include "Composition.h"
#include "Transform.h"

namespace Framework
{

  class Boid : public GameComponent
  {
    public: 
      Boid(void);
      ~Boid(void);
      void Serialize(ISerializer& stream);
      void Initialize(void);
      void Update(float timeslice);

    //private:
      void MoveToGoal(void);
      void SAC(void);
      void ObstacleAvoidance(void);
      void SetBoidGoal(int x, int y, int z);
      void SetBoidGoal(Vec3 pos);
      bool BoidAtGoal(void);

      Vec3 CurrGoal;
      Vec3 BoidPos;
      Vec3 BoidDir;
      Vec3 BoidVel;
      Vec3 BoidAccel;
      float VelDecay;

      float MaxSpeed;
      float MinSpeed;

      Ray3D Feeler;
      
      int CurrState;
      bool FindTarget;
      Transform * transform;

  };

}

#endif