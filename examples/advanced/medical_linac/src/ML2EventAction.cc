//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// The code was written by :
//	^Claudio Andenna claudio.andenna@iss.infn.it, claudio.andenna@ispesl.it
//      *Barbara Caccia barbara.caccia@iss.it
//      with the support of Pablo Cirrone (LNS, INFN Catania Italy)
//
// ^ISPESL and INFN Roma, gruppo collegato Sanità, Italy
// *Istituto Superiore di Sanità and INFN Roma, gruppo collegato Sanità, Italy
//  Viale Regina Elena 299, 00161 Roma (Italy)
//  tel (39) 06 49902246
//  fax (39) 06 49387075
//
// more information:
// http://g4advancedexamples.lngs.infn.it/Examples/medical-linac
//
//*******************************************************//


#include "ML2EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4SDManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"


CML2EventAction::CML2EventAction() :
  drawFlag("all" )
{
 }

 
CML2EventAction::~CML2EventAction()
{
 }
 
void CML2EventAction::BeginOfEventAction(const G4Event*)
{
}

 
void CML2EventAction::EndOfEventAction(const G4Event* evt)
{  
 // extract the trajectories and draw them ...

  if (G4VVisManager::GetConcreteInstance())
    {
      G4TrajectoryContainer * trajectoryContainer = evt->GetTrajectoryContainer();
      G4int n_trajectories = 0;
      if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

      for (G4int i=0; i<n_trajectories; i++) 
        {
			G4Trajectory* trj = (G4Trajectory*)
			((*(evt->GetTrajectoryContainer()))[i]);
			if(drawFlag == "all") trj->DrawTrajectory(50);
			else if((drawFlag == "charged")&&(trj->GetCharge() != 0.))
			trj->DrawTrajectory(50);
			else if ((drawFlag == "neutral")&&(trj->GetCharge() == 0.))
			trj->DrawTrajectory(50);	


		}
    }
 }
