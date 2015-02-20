#include "Precompiled.h"
#include "Boid.h"
#include "GameLogic.h"
#include "Physics.h"
#include "Body.h"
#include <time.h>

namespace Framework
{

  Boid::Boid(void)
  {
	CurrGoal.Set(0);
	BoidPos.Set(0);
	BoidDir.Set(-1,-1,-1);
	BoidVel.Set(5.0f);
    BoidAccel.Set(0.0f);
	Feeler.dir.Set(0,0,-1);
	Feeler.origin = BoidPos;
    FindTarget = false;

	VelDecay = .0001;
	MaxSpeed = 10.0f;
	MinSpeed = 5.0f;
	LOGIC->MoveToPoint.Set(0,0,0);
  }

  Boid::~Boid(void)
  {
    LOGIC->RemoveBoid(this);
  }

  void Boid::Serialize(ISerializer& stream)
  {
  }

  void Boid::Initialize(void)
  {
	transform = GetOwner()->has(Transform);
	//BoidPos = transform->Position;
	//Feeler.origin = transform->Position + 10;
	//Feeler.dir    = Feeler.origin.z - 10;
    LOGIC->AddBoid(this);
  }

  void Boid::Update(float dt)
  {
	//std::cout << transform->Position.z << std::endl;
	//update velocity and position
	Vec3 OldVel = BoidVel;
	BoidVel += BoidAccel * dt;
	if(BoidVel.MagSqrd() > (MaxSpeed*MaxSpeed)) 
	{
      BoidVel.Normalize();
      BoidVel *= MaxSpeed;
    }
	else if(BoidVel.MagSqrd() < (MinSpeed*MinSpeed))
	{
	  BoidVel.Normalize();
      BoidVel *= MinSpeed;
	}

    Vec3 NormVelocity;
	BoidVel.Norm(&NormVelocity);
	Vec3 rot_axis;
	OldVel.Normalize();
	OldVel.Cross(NormVelocity,&rot_axis);
	float angle = OldVel.Dot(NormVelocity);
	angle = min(angle, 1.0f);
	angle = max(angle,-1.0f);
	angle = acos(angle);

	if(angle > Epsilon)
	{
	  rot_axis.Normalize();
	  Mtx44 m = Mtx44Rotate(rot_axis,angle);

	  transform->Dir = Mtx44MultPoint(transform->Dir,m);
	  transform->Up  = Mtx44MultPoint(transform->Up,m);
	  transform->Right = Mtx44MultPoint(transform->Right,m);

	  transform->Dir.Normalize();
	  transform->Up.Normalize();
	  transform->Right.Normalize();
	}


	transform->Position += (BoidVel * -1);
	
	Feeler.origin = transform->Position;
	Feeler.dir = BoidVel;
	Feeler.dir.Negate();

	MoveToGoal();
    BoidAccel.Set(0);

	ObstacleAvoidance();
    SAC();
  }
  
  void Boid::MoveToGoal(void)
  {
    Vec3 DirToGoal      = LOGIC->MoveToPoint - transform->Position;
	float DistToGoal    = DirToGoal .Magnitude();
    float MaxDistance   = LOGIC->MaxDistanceFromGoal;

    if(DistToGoal > MaxDistance)
	{
      float PullStrength = 0.0001f;
      DirToGoal.Normalize();
	  BoidVel -= (DirToGoal * ((DistToGoal - MaxDistance) * PullStrength));
    }
  }

  void Boid::SAC()
  {
    std::vector<Boid*>::iterator b1;
	for(b1 = LOGIC->boids.begin();b1 != LOGIC->boids.end(); ++b1)
	{
	  if(*b1 == this)
	    continue;

	  Vec3 dir = this->transform->Position - (*b1)->transform->Position;
	  float distance_sqrd = dir.MagSqrd();
	  float n_rsqrd = (LOGIC->NeighborRadius*LOGIC->NeighborRadius);

	  if(distance_sqrd < n_rsqrd)
	  {
		float percent = (distance_sqrd/n_rsqrd); 
	    if(percent == 0) // on top of eachother
		{
		  this->BoidAccel   += -0.01f;
		  (*b1)->BoidAccel  -= -0.01f;
		}
		else if(percent < LOGIC->SeperationDistance)//SEPERATION
		{
	      float Force = (LOGIC->SeperationDistance/percent - 1.0f) * 0.01f;
		  dir.Normalize();
		  dir *= Force;
		  this->BoidAccel  += dir;
		  (*b1)->BoidAccel -= dir;
		}
		else if(percent < LOGIC->AlignmentDistance)  //ALLIGNMENT
		{
		  float thresh_delta = LOGIC->AlignmentDistance - LOGIC->SeperationDistance;
		  float adjusted_percent = (percent - LOGIC->SeperationDistance)/thresh_delta;
		  float Force = (.5 -(cos(adjusted_percent*PI*2.0f)* 0.5f + 0.5f)) * 0.04f;
          dir.Normalize();
		  dir *= Force;
          this->BoidAccel  += dir;
          (*b1)->BoidAccel += dir;
		}
		else//COHESION
		{
          float thresh_delta = 1.0f - LOGIC->AlignmentDistance;
          float adjusted_percent = ( percent - LOGIC->AlignmentDistance )/thresh_delta;
          float Force = (.5 -(cos(adjusted_percent*PI*2.0f)* 0.5f + 0.5f)) * 0.04f;
          dir.Normalize();
		  dir *= Force;
          this->BoidAccel  -= dir;
          (*b1)->BoidAccel += dir;
		}

	  }
	}
  }

  void Boid::ObstacleAvoidance(void)
  {
	Vec3 po;
	float pt;
	Feeler.dir *= 100;
    //update all the bodies
	for(Body* curr_body = PHYSICS->BodyList; curr_body != NULL; curr_body = curr_body->next)
	{
      if(curr_body->GetOwner()->has(SphereBody))
	    if(Intersect(Feeler, curr_body->GetOwner()->has(SphereBody)->CollisionData, &po, &pt))
		{
		  BoidAccel += curr_body->GetOwner()->has(SphereBody)->CollisionData.radius + 100;
		}
	}
	Feeler.dir /= 100;
  }

  void Boid::SetBoidGoal(int x, int y, int z)
  {
    CurrGoal.x = x;
    CurrGoal.y = y;
	CurrGoal.z = z;
  }

  void Boid::SetBoidGoal(Vec3 pos)
  {
    CurrGoal = pos;
  }

  bool Boid::BoidAtGoal(void)
  {
    return CurrGoal == BoidPos;
  }

}//namespace
