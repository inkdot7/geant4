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
//
// $Id: G4EvaporationLevelDensityParameter.cc,v 1.6 2008-05-08 09:59:37 quesada Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (Oct 1998)
//


#include "G4EvaporationLevelDensityParameter.hh"
#include "G4HadronicException.hh"

// Those values are from table 3 in 
// A.S. Iljinov et al. Nucl Phys A543 (1992) 517-557
// Table 3. alpha, beta and gamma for Cameron Shell corrections
// whithout collective effects. f-factor = 2.31.

//JMQ 17-04-08 these are not used at present in G4Evaporation 
const G4double G4EvaporationLevelDensityParameter::ConstEvapLevelDensityParameter = 0.125*(1./MeV);
//const G4double G4EvaporationLevelDensityParameter::ConstEvapLevelDensityParameter= 0.0769231*(1./MeV);
const G4double G4EvaporationLevelDensityParameter::alpha = 0.072*(1./MeV);
const G4double G4EvaporationLevelDensityParameter::beta = 0.257*(1./MeV);
const G4double G4EvaporationLevelDensityParameter::gamma = 0.059*(1./MeV);
const G4double G4EvaporationLevelDensityParameter::Bs = 1.0;


G4EvaporationLevelDensityParameter::
G4EvaporationLevelDensityParameter(const G4EvaporationLevelDensityParameter &) : G4VLevelDensityParameter()
{
    throw G4HadronicException(__FILE__, __LINE__, "G4EvaporationLevelDensityParameter::copy_constructor meant to not be accessable");
}


const G4EvaporationLevelDensityParameter & G4EvaporationLevelDensityParameter::
operator=(const G4EvaporationLevelDensityParameter &)
{
    throw G4HadronicException(__FILE__, __LINE__, "G4EvaporationLevelDensityParameter::operator= meant to not be accessable");
    return *this;
}


G4bool G4EvaporationLevelDensityParameter::operator==(const G4EvaporationLevelDensityParameter &) const
{
    return false;
}

G4bool G4EvaporationLevelDensityParameter::operator!=(const G4EvaporationLevelDensityParameter &) const
{
    return true;
}

G4double G4EvaporationLevelDensityParameter::LevelDensityParameter(const G4int A,const G4int Z,
								   const G4double U) const 
//JMQ (Apr .08) this is the method used in G4Evaporation 
{

//JMQ 25/04/08  a=A/10 according to original Gudima's prescription
 G4double a=static_cast<G4double>(A)/10.;
    return a;
//

//    G4int N = A - Z;

    // Asymptotic Level Density Parameter
//    G4double AsymptoticLDP = (alpha*static_cast<G4double>(A) + beta*std::pow(static_cast<G4double>(A),2./3.)*Bs)/MeV;
	
    // Shape of the LDP U dependence
//    G4double exponent = -gamma*U;
//    G4double f = 1.;
//    if (exponent > -300.) f -= std::exp(exponent);
	
    // Level Density Parameter
//    G4double a = AsymptoticLDP*(1. + ShellCorrection(Z,N)*f/U);
//    return a;
}

