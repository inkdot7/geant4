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
//
// $Id: G4LogicalVolume.cc,v 1.18 2003-11-02 14:01:23 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// class G4LogicalVolume Implementation
//
// History:
// 17.05.02 G.Cosmo: Added flag for optional optimisation
// 12.02.99 S.Giani: Default initialization of voxelization quality
// 04.08.97 P.M.DeFreitas: Added methods for parameterised simulation 
// 19.08.96 P.Kent: Modified for G4VSensitive Detector
// 11.07.95 P.Kent: Initial version
// --------------------------------------------------------------------

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"

// ********************************************************************
// Constructor - sets member data and adds to logical Store,
//               voxel pointer for optimisation set to 0 by default.
//               Initialises daughter vector to 0 length.
// ********************************************************************
//
G4LogicalVolume::G4LogicalVolume( G4VSolid* pSolid,
                                  G4Material* pMaterial,
                            const G4String& name,
                                  G4FieldManager* pFieldMgr,
                                  G4VSensitiveDetector* pSDetector,
                                  G4UserLimits* pULimits,
                                  G4bool optimise )
 : fDaughters(0,(G4VPhysicalVolume*)0), fFieldManager(pFieldMgr),
   fVoxel(0), fOptimise(optimise), fRootRegion(false), fSmartless(2.),
   fVisAttributes(0), fFastSimulationManager(0), fRegion(0),
   fCutsCouple(0), fIsEnvelope(false)
{
  SetSolid(pSolid);
  SetMaterial(pMaterial);
  SetName(name);
  SetSensitiveDetector(pSDetector);
  SetUserLimits(pULimits);    
  //
  // Add to solid Store
  //
  G4LogicalVolumeStore::Register(this);
}

// ********************************************************************
// Destructor - Removes itself from solid Store
// NOTE: Not virtual
// ********************************************************************
//
G4LogicalVolume::~G4LogicalVolume()
{
  if(fRootRegion) fRegion->RemoveRootLogicalVolume(this);
  G4LogicalVolumeStore::DeRegister(this);
}

// ********************************************************************
// SetFastSimulationManager
//
// NOTE: recursive method, not inlined.
// ********************************************************************
//
void
G4LogicalVolume::
SetFastSimulationManager( G4FastSimulationManager* pNewFastSimul,
                          G4bool IsEnvelope ) 
{
  if( !fIsEnvelope || IsEnvelope )
  {
    fIsEnvelope = IsEnvelope;
    fFastSimulationManager = pNewFastSimul;

    G4int NoDaughters = GetNoDaughters();
    while ( (NoDaughters--)>0 )
    {
      G4LogicalVolume* DaughterLogVol; 
      DaughterLogVol = GetDaughter(NoDaughters)->GetLogicalVolume();
      if( DaughterLogVol->GetFastSimulationManager() != pNewFastSimul )
      {
        DaughterLogVol->SetFastSimulationManager(pNewFastSimul,false);
      }
    }
  }
}

// ********************************************************************
// ClearEnvelopeForFastSimulation
// ********************************************************************
//
void
G4LogicalVolume::ClearEnvelopeForFastSimulation( G4LogicalVolume* motherLogVol )
{
  if( fIsEnvelope )
  {
    G4FastSimulationManager* NewFastSimulationVal = 0;

    // This is no longer an envelope !
    //
    fIsEnvelope = false;

    if( motherLogVol == 0 )
    {
      motherLogVol = this->FindMotherLogicalVolumeForEnvelope();
    }

    // Reset its ParameterisedSimulation values and those of all daughters
    // (after ensuring the mother was given correctly or was found)
    //
    if( motherLogVol != 0 )
    {
      NewFastSimulationVal = motherLogVol->GetFastSimulationManager();
      SetFastSimulationManager(NewFastSimulationVal, false);
    }
  }
  else
  {
    G4cerr << "ERROR - Called ClearEnvelope() for non-envelope logical volume!"
           << G4endl;
    G4Exception("G4LogicalVolume::ClearEnvelopeForFastSimulation()",
                "NotApplicable", FatalException,
		"Cannot be called for non-envelope logical volumes.");
  }
}

// ********************************************************************
// SetFieldManager
// ********************************************************************
//
void
G4LogicalVolume::SetFieldManager(G4FieldManager* pNewFieldMgr,
                                 G4bool          forceAllDaughters) 
{
  fFieldManager = pNewFieldMgr;

  G4int NoDaughters = GetNoDaughters();
  while ( (NoDaughters--)>0 )
  {
    G4LogicalVolume* DaughterLogVol; 
    DaughterLogVol = GetDaughter(NoDaughters)->GetLogicalVolume();
    if ( forceAllDaughters || (DaughterLogVol->GetFieldManager() == 0) )
    {
      DaughterLogVol->SetFieldManager(pNewFieldMgr, forceAllDaughters);
    }
  }
}


// ********************************************************************
// IsAncestor
//
// Finds out if the current logical volume is an ancestor of a given 
// physical volume
// ********************************************************************
//
G4bool
G4LogicalVolume::IsAncestor(const G4VPhysicalVolume* aVolume) const
{
  G4bool isDaughter = IsDaughter(aVolume);
  if (!isDaughter)
  {
    for (G4PhysicalVolumeList::const_iterator itDau = fDaughters.begin();
         itDau != fDaughters.end(); itDau++)
    {
      isDaughter = (*itDau)->GetLogicalVolume()->IsAncestor(aVolume);
      if (isDaughter)  break;
    }
  }
  return isDaughter;
}


// ********************************************************************
// FindMotherLogicalVolumeForEnvelope
//
// Returns a meaningful result IF and only IF the current logical
// volume has exactly one physical volume that uses it.
// ********************************************************************
//
G4LogicalVolume* 
G4LogicalVolume::FindMotherLogicalVolumeForEnvelope()
{
  G4LogicalVolume* motherLogVol = 0;
  G4LogicalVolumeStore* Store = G4LogicalVolumeStore::GetInstance();

  // Look for the current volume's mother volume.
  //
  for ( size_t LV=0; LV < Store->size(); LV++ )
  {
    G4LogicalVolume* aLogVol = (*Store)[LV]; // Don't look for it inside itself!
    if( (aLogVol!=this) && (aLogVol->GetFastSimulationManager()!=0) )
    {
      for ( G4int daughter=0; daughter<aLogVol->GetNoDaughters(); daughter++ )
      {
        if( aLogVol->GetDaughter(daughter)->GetLogicalVolume()==this )
        { 
          // aLogVol is the mother !!!
          //
          motherLogVol = aLogVol;
          break;
        }
      }
    }
  }
  return motherLogVol;
}
