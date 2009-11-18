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


#include "ML2PhaseSpaces.hh"
#include "ML2ReadOutGeometry.hh"

CML2PhaseSpaces::CML2PhaseSpaces():sensDetParticle(0), sensDetVoxelized(0)
{}

CML2PhaseSpaces::~CML2PhaseSpaces(void)
{}

bool CML2PhaseSpaces::createPlane(G4VPhysicalVolume  *PVWorld, G4String name, G4ThreeVector centre, G4ThreeVector halfSize)
{
	// constructor for killer plane
	bool bCreated=false;
	G4Material *Vacum=G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
	G4Box *box;
	G4LogicalVolume *logVol;
	G4VPhysicalVolume *phVol;
	box = new G4Box("KBox", halfSize.getX(), halfSize.getY(), halfSize.getZ());
	logVol = new G4LogicalVolume(box, Vacum, name+"KLV", 0, 0, 0);
	phVol= new G4PVPlacement(0, centre, name+"KPV", logVol, PVWorld, false, 0);

	G4VisAttributes* simplePhSpVisAtt= new G4VisAttributes(G4Colour::Yellow());
	simplePhSpVisAtt->SetVisibility(true);
	simplePhSpVisAtt->SetForceSolid(true);
	logVol->SetVisAttributes(simplePhSpVisAtt);


	this->sensDetParticle=new CML2SDWithParticle();
	G4SDManager *SDManager=G4SDManager::GetSDMpointer();
	SDManager->AddNewDetector(this->sensDetParticle);
	logVol->SetSensitiveDetector(this->sensDetParticle);
	bCreated=true;
	return bCreated;
}

bool CML2PhaseSpaces::createPlane(G4int idSD_Type, G4int max_N_particles_in_PhSp_File, G4int seed, G4int nMaxParticlesInRamPhaseSpace, G4VPhysicalVolume  *PVWorld, G4String name, G4String PhaseSpaceOutFile, G4bool bSavePhaseSpace, G4bool bStopAtPhaseSpace, G4ThreeVector centre, G4ThreeVector halfSize, SPrimaryParticle *primaryParticleData)
{
	// constructor for phase space plane
	bool bCreated=false;
	G4Material *Vacum=G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
	G4Box *box;
	G4LogicalVolume *logVol;
	G4VPhysicalVolume *phVol;
	box = new G4Box(name+"Box", halfSize.getX(), halfSize.getY(), halfSize.getZ());
	logVol = new G4LogicalVolume(box, Vacum, name+"LV", 0, 0, 0);
	phVol= new G4PVPlacement(0, centre, name+"PV", logVol, PVWorld, false, 0);

	G4VisAttributes* simplePhSpVisAtt= new G4VisAttributes(G4Colour::Yellow());
	simplePhSpVisAtt->SetVisibility(true);
	simplePhSpVisAtt->SetForceSolid(true);
	logVol->SetVisAttributes(simplePhSpVisAtt);

	this->sensDetParticle=new CML2SDWithParticle(idSD_Type, max_N_particles_in_PhSp_File, seed, nMaxParticlesInRamPhaseSpace, name, PhaseSpaceOutFile, bSavePhaseSpace, bStopAtPhaseSpace, primaryParticleData);
	G4SDManager *SDManager=G4SDManager::GetSDMpointer();
	SDManager->AddNewDetector(this->sensDetParticle);
	logVol->SetSensitiveDetector(this->sensDetParticle);
	bCreated=true;
	return bCreated;
}

