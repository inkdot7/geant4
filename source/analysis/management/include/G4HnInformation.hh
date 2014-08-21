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
// $Id$

// Data class for the added Hn/Pn information (not available in g4tools). 
//
// Author: Ivana Hrivnacova, 04/07/2012  (ivana@ipno.in2p3.fr)

#ifndef G4HnInformation_h
#define G4HnInformation_h 1

#include "globals.hh"
#include "G4Fcn.hh" 
#include "G4BinScheme.hh" 

// The additional Hn information per dimension

struct G4HnDimensionInformation
{
  G4HnDimensionInformation( 
      const G4String& unitName,
      const G4String& fcnName,
      G4double unit, 
      G4Fcn fcn,
      G4BinScheme binScheme) 
    : fUnitName(unitName), 
      fFcnName(fcnName),
      fUnit(unit), 
      fFcn(fcn),
      fBinScheme(binScheme)
      {}
      
  G4HnDimensionInformation(const G4HnDimensionInformation& rhs) 
    : fUnitName(rhs.fUnitName), 
      fFcnName(rhs.fName),
      fUnit(rhs.fUnit), 
      fFcn(rhs.fFcn),
      fBinScheme(rhs.fBinScheme)
      {}

  G4String fName;
  G4String fUnitName;
  G4String fFcnName;
  G4double fUnit;  
  G4Fcn    fFcn;
  G4BinScheme fBinScheme;
};  

class G4HnInformation
{
  public:
    enum {
      kX = 0,
      kY = 1,
      kZ = 2
    };  

  public:
    G4HnInformation(const G4String& name, G4int nofDimensions)
      : fName(name),
        fHnDimensionInformations(),
        fActivation(true),
        fAscii(false) {fHnDimensionInformations.reserve(nofDimensions); }
  
    // Set methods
    void AddHnDimensionInformation(
            const G4HnDimensionInformation& hnDimensionInformation);
    void SetActivation(G4bool activation);
    void SetAscii(G4bool ascii);  
  
    // Get methods
    G4String GetName() const;
    G4HnDimensionInformation* GetHnDimensionInformation(G4int dimension);
    G4bool  GetActivation() const;
    G4bool  GetAscii() const;  

  private:
    // Disabled default constructor
    G4HnInformation(); 
    
    // Data members
    G4String fName;
    std::vector<G4HnDimensionInformation> fHnDimensionInformations;
    G4bool   fActivation;
    G4bool   fAscii;  
};

// inline functions

inline void G4HnInformation::AddHnDimensionInformation(
                const G4HnDimensionInformation& hnDimensionInformation)
{ fHnDimensionInformations.push_back(hnDimensionInformation); }

inline void G4HnInformation::SetActivation(G4bool activation)
{ fActivation = activation; }

inline void G4HnInformation::SetAscii(G4bool ascii)
{ fAscii = ascii; }

inline G4String G4HnInformation::GetName() const
{ return fName; }

inline G4HnDimensionInformation* G4HnInformation::GetHnDimensionInformation(G4int dimension)
{ return &(fHnDimensionInformations[dimension]); }

inline G4bool  G4HnInformation::GetActivation() const
{ return fActivation; }

inline G4bool  G4HnInformation::GetAscii() const
{ return fAscii; }

#endif  
