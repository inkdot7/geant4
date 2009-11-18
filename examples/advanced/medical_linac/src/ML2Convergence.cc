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


#include "ML2Convergence.hh"

CML2Convergence::CML2Convergence(G4int seed, G4int saving_in_Selected_Voxels_every_events, G4String FileExperimentalData, G4bool bCompareExp, G4int minNumberOfEvents)
:ML2ExpVoxels(0)
{
	this->bCompareExp=bCompareExp;
	this->fileExperimentalData=FileExperimentalData;

// if the flag compareExp if true and the experimental data is given create the class CML2ExpVoxels
	if (this->bCompareExp && this->fileExperimentalData!="")
	{
		this->ML2ExpVoxels=new CML2ExpVoxels(this->bCompareExp, saving_in_Selected_Voxels_every_events, seed, FileExperimentalData);
		if (!this->ML2ExpVoxels->loadData())
		{
			this->ML2ExpVoxels=0;
		}
	}
	this->minNumberOfEvents=minNumberOfEvents;
}

CML2Convergence::~CML2Convergence(void)
{
	if (this->ML2ExpVoxels!=0)
	{delete this->ML2ExpVoxels;}

}
void CML2Convergence::add(const G4Step* aStep)
{
// accumulate events in the CML2ExpVoxels class (if created)
	if (this->ML2ExpVoxels!=0)
	{
		G4double energyDep=aStep->GetTotalEnergyDeposit();
		if (energyDep>0.)
		{
			G4double density=aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume()->GetMaterial()->GetDensity();
			this->ML2ExpVoxels->add(aStep->GetPreStepPoint()->GetPosition(), energyDep, density);
		}
	}
}
G4bool CML2Convergence::runAgain()
{
	G4bool bAgain=true;
	if (this->ML2ExpVoxels!=0)
	{
		bAgain=!this->convergenceCriteria();
		return bAgain;
	}
	return bAgain;
}
G4bool CML2Convergence::convergenceCriteria()
{
	if (this->bCompareExp)
	{
		if (this->ML2ExpVoxels->getMinNumberOfEvents() > this->minNumberOfEvents)
		{return false;}
		else
		{return true;}
	}
	return false;
}
