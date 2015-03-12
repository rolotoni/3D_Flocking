///////////////////////////////////////////////////////////////////////////////////////
//
//  Game Logic
//  Simple game logic for demo. A good way to approach understand the game sample
//  is to walk through all the code in game logic and follow the execution through
//  the rest of the engine.
//  
//  Authors: Chris Peters, Benjamin Ellinger
//  Copyright 2010, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////

#include "Precompiled.h"
#include "GameLogic.h"
#include "SDLWindowSystem.h"
#include "SDLInput.h"
#include "Core.h"
#include "ComponentCreator.h"
#include "Camera.h"
#include "TextSerialization.h"
#include "Physics.h"
#include "ObjectInfo.h"
#include "Body.h"
#include "Boid.h"
#include <ctime>

namespace Framework
{
    //Our global pointer to the game.
    GameLogic* LOGIC = NULL;

    void GameLogic::Initialize()
    {
       //GS_Level
       //GS_MainMenu
       //Components are explicitly registered in initialize
       RegisterComponent(Transform);
       RegisterComponent(Model);
       RegisterComponent(Controller);
       RegisterComponent(ObjectInfo);
       RegisterComponent(AABBBody);
       RegisterComponent(Boid);

       ObjectInfoList = NULL;
       ObjectInfoListEnd = NULL;

       LoadLevelFile("data//scenes//Flock.txt");
       NeighborRadius = 50.0f;
       SeperationDistance = .45f;
       AlignmentDistance= .75f;
       MaxDistanceFromGoal = 100.0f;
       MoveToPoint.Set(0);
       Paused = false;
    }

    GameLogic::GameLogic()
    {
       //Safe Id reference of the object the user has grabbed
       GrabbedObjectId = 0;
       //Set up the global pointer
       //HALT("Logic already initialized",LOGIC!=NULL);
       LOGIC = this;
    }

    GameLogic::~GameLogic()
    {
    }

    void GameLogic::RecieveMessage(Message * m)
    {
      switch(m->MessageId)
      {
        case Mid::CharacterKey:
        {
          // Cast to the derived message type
          MessageCharacterKey * key = (MessageCharacterKey*)m;
          break;
        }

        case Mid::MouseWheel:
        {
          MouseWheel * mw = (MouseWheel*)m;
          break;
        }
      
        case Mid::MouseButton:
        {
          MouseButton* mb = (MouseButton*)m;
          if(mb->MouseButtonIndex == mb->LeftMouse && mb->ButtonIsPressed)
          {
            //MoveToPoint = GRAPHICS->ScreenToWorldSpace(mb->MousePosition);
            MoveToPoint.x = mb->MousePosition.x - GRAPHICS->ScreenWidth;
            MoveToPoint.y = -(mb->MousePosition.y - GRAPHICS->ScreenHeight);
            MoveToPoint.z = 0;
          }
          break;
        }
        case Mid::MouseMove:
        {
          MouseMove* mouse = (MouseMove*)m;
          MouseX = mouse->MousePosition.x;
          MouseY = mouse->MousePosition.y;
          break;
        }
      }

    };

    void GameLogic::Update(float dt)
    {
        //update controllers
        ObjectLinkList<Controller>::iterator it = Controllers.begin();
        for(;it!=Controllers.end();++it)
          it->Update(dt);

        //update object infos
        for(ObjectInfo* curr= ObjectInfoList; curr != NULL; curr = curr->Next)
          curr->Update(dt);

        //update the boids
        if(!Paused)
          for(int i = 0; i < boids.size(); ++i)
            if(boids[i] != NULL)
              boids[i]->Update(dt);

        //update the camera
        CamLogic();

        //updateplayercursor
        //GetPlayer()->MoveCursor(x, y);

    }

    GOC * GameLogic::CreateObjectAt(Vec3& position,Vec3& RotAxis,float rotation,const std::string& file)
    {
       //This is an example of using some data out of data file using
       //serialization and then override fields before the object
       //is initialized.

       //Build and serialize the object with data from the file
       GOC * newObject = FACTORY->BuildAndSerialize(file);
       //Get the transform and adjust the position
       Transform * transform = newObject->has(Transform);
       transform->Position = position;
       transform->RotAngle = rotation;
       transform->RotAxis  = RotAxis;

       //Initialize the composition
       newObject->Initialize();

       return newObject;
    }

    void GameLogic::LoadLevelFile(const std::string& filename)
    {
       TextSerializer stream;
       bool fileOpened = stream.Open(filename);
       if(!fileOpened)
         HALT("Could not open file %s. File does not exist or is protected." , filename.c_str());

       std::string objectArchetype;
       Vec3 objectPosition;
       Vec3 objectRotAxis;
       float objectRotation;

       while(stream.IsGood())
       {
          StreamRead(stream,objectArchetype);
          StreamRead(stream,objectPosition);
          StreamRead(stream,objectRotAxis);
          StreamRead(stream,objectRotation);
          CreateObjectAt(objectPosition,objectRotAxis,objectRotation,"data\\objects\\" + objectArchetype);
       }
    }

    void GameLogic::ResolveCollision(GOC* obj1, GOC* obj2)
    {
      if(obj1->has(ObjectInfo) && obj1->has(ObjectInfo)->GetObjectType() == CANNON_BALL)
      {
        if(obj2->has(ObjectInfo) && obj2->has(ObjectInfo)->GetObjectType() == WALL)
          if(obj1->has(ObjectInfo)->GetTeam() != obj2->has(ObjectInfo)->GetTeam())
            obj2->has(ObjectInfo)->Health -= 100.0f;
              if(obj2->has(ObjectInfo)->GetObjectType() != CANNON_BALL)
                obj1->has(ObjectInfo)->Health -= 100.0f;
            }
    }

    void GameLogic::AddObjectInfo(ObjectInfo* objinfo)
    {
        if(ObjectInfoListEnd == NULL)
        {
          ObjectInfoList = objinfo;
          ObjectInfoListEnd = objinfo;
        }
        else
        {
          ObjectInfoListEnd->Next = objinfo;
          objinfo->Prev = ObjectInfoListEnd;
          ObjectInfoListEnd = objinfo;
        }
    }

    void GameLogic::RemoveObjectInfo(ObjectInfo* objinfo)
    {
        if(objinfo == ObjectInfoList)
          ObjectInfoList = objinfo->Next;
        if(objinfo == ObjectInfoListEnd)
          ObjectInfoListEnd = objinfo->Prev;

        if(objinfo->Next != NULL)
          objinfo->Next->Prev = objinfo->Prev;
        if(objinfo->Prev != NULL)
          objinfo->Prev->Next = objinfo->Next;

        objinfo->Next = NULL;
        objinfo->Prev = NULL;
    }
    
    void GameLogic::AddBoid(Boid* boid)
    {
      boids.push_back(boid);
    }

    void GameLogic::RemoveBoid(Boid* boid)
    {
      std::vector<Boid*>::iterator it = boids.begin();
      for(; it != boids.end(); ++it)
      {
        if(*it == boid)
        {
          (*it)->GetOwner()->Destroy();
          boids.erase(it);
          return;
        }
      }
    }

    void GameLogic::CamLogic(void)
    {
    }

    void GameLogic::ClearLogicBoard(void)
    {
    }

    Controller::Controller()
    {
        //Set the default speed
        Speed = 50.0f;
    }

    Controller::~Controller()
    {
        LOGIC->Controllers.erase( this );
    }

    void Controller::Initialize()
    { 
        transform = GetOwner()->has(Transform);
        LOGIC->Controllers.push_back( this );
    }

    void Controller::Update(float dt)
    {
    }

    void Controller::Serialize(ISerializer& stream)
    {
        StreamRead(stream,Speed);
    }
    
}



