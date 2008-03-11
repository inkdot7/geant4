//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// History:
// - Created.                                  Zoltan Torzsok, November 2007
// -------------------------------------------------------------------------

#ifndef _G4GDMLREADSTRUCTURE_INCLUDED_
#define _G4GDMLREADSTRUCTURE_INCLUDED_

#include "G4AssemblyVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4PVDivision.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SolidStore.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ReflectionFactory.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalBorderSurface.hh"

#include "G4GDMLReadParamvol.hh"

struct G4GDMLAuxPairType {

   G4String type;
   G4double value;
};

typedef std::vector<G4GDMLAuxPairType> G4GDMLAuxListType;
typedef std::map<G4LogicalVolume*,G4GDMLAuxListType> G4GDMLAuxMapType;

class G4GDMLReadStructure : public G4GDMLReadParamvol {
private:
   G4GDMLAuxMapType auxMap;

   G4AssemblyVolume *pAssembly;
   G4LogicalVolume *pMotherLogical;

   std::map<G4String,G4AssemblyVolume*> assemblyMap;

   void GeneratePhysvolName(G4VPhysicalVolume*);
   void assemblyRead(const xercesc::DOMElement* const);
   G4GDMLAuxPairType auxiliaryRead(const xercesc::DOMElement* const);
   void bordersurfaceRead(const xercesc::DOMElement* const);
   void divisionvolRead(const xercesc::DOMElement* const);
   G4LogicalVolume* fileRead(const xercesc::DOMElement* const);
   void physvolRead(const xercesc::DOMElement* const);
   void replicavolRead(const xercesc::DOMElement* const);
   void skinsurfaceRead(const xercesc::DOMElement* const);
   void volumeRead(const xercesc::DOMElement* const);
   void volume_contentRead(const xercesc::DOMElement* const);
   void structureRead(const xercesc::DOMElement* const);
public:
   G4VPhysicalVolume* getPhysvol(const G4String&) const;
   G4LogicalVolume* getVolume(const G4String&) const;
   G4GDMLAuxListType getVolumeAuxiliaryInformation(G4LogicalVolume*);
};

#endif
