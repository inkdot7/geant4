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
// $Id: G4StatMFMacroTemperature.cc,v 1.6 2008-07-25 11:20:47 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara


#include "G4StatMFMacroTemperature.hh"

// operators definitions
G4StatMFMacroTemperature & 
G4StatMFMacroTemperature::operator=(const G4StatMFMacroTemperature & ) 
{
    throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::operator= meant to not be accessable");
    return *this;
}

G4bool G4StatMFMacroTemperature::operator==(const G4StatMFMacroTemperature & ) const 
{
    throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::operator== meant to not be accessable");
    return false;
}


G4bool G4StatMFMacroTemperature::operator!=(const G4StatMFMacroTemperature & ) const 
{
    throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::operator!= meant to not be accessable");
    return true;
}




G4double G4StatMFMacroTemperature::CalcTemperature(void) 
{
    // Inital guess for the interval of the ensemble temperature values
    G4double Ta = 0.5; 
    G4double Tb = std::max(std::sqrt(_ExEnergy/(theA*0.12)),0.01*MeV);
    
    G4double fTa = this->operator()(Ta); 
    G4double fTb = this->operator()(Tb); 

    // Bracketing the solution
    // T should be greater than 0.
    // The interval is [Ta,Tb]
    // We start with a value for Ta = 0.5 MeV
    // it should be enough to have fTa > 0 If it isn't 
    // the case, we decrease Ta. But carefully, because 
    // fTa growes very fast when Ta is near 0 and we could have
    // an overflow.

    G4int iterations = 0;  
    while (fTa < 0.0 && iterations++ < 10) {
	Ta -= 0.5*Ta;
	fTa = this->operator()(Ta);
    }
    // Usually, fTb will be less than 0, but if it is not the case: 
    iterations = 0;  
    while (fTa*fTb > 0.0 && iterations++ < 10) {
	Tb += 2.*std::abs(Tb-Ta);
	fTb = this->operator()(Tb);
    }
	
    if (fTa*fTb > 0.0) {
      G4cerr <<"G4StatMFMacroTemperature:"<<" Ta="<<Ta<<" Tb="<<Tb<< G4endl;
      G4cerr <<"G4StatMFMacroTemperature:"<<" fTa="<<fTa<<" fTb="<<fTb<< G4endl;
	throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::CalcTemperature: I couldn't bracket the solution.");
    }

    G4Solver<G4StatMFMacroTemperature> * theSolver = new G4Solver<G4StatMFMacroTemperature>(100,1.e-4);
    theSolver->SetIntervalLimits(Ta,Tb);
    if (!theSolver->Crenshaw(*this)){ 
      G4cerr <<"G4StatMFMacroTemperature, Crenshaw method failed:"<<" Ta="<<Ta<<" Tb="<<Tb<< G4endl;
      G4cerr <<"G4StatMFMacroTemperature, Crenshaw method failed:"<<" fTa="<<fTa<<" fTb="<<fTb<< G4endl;
    }
    _MeanTemperature = theSolver->GetRoot();
    G4double FunctionValureAtRoot =  this->operator()(_MeanTemperature);
    delete  theSolver;

    // Verify if the root is found and it is indeed within the physical domain, 
    // say, between 1 and 50 MeV, otherwise try Brent method:
    if (_MeanTemperature < 1. || _MeanTemperature > 50. || std::abs(FunctionValureAtRoot) > 5.e-2) {
    G4cout << "Crenshaw method failed; function = " << FunctionValureAtRoot << " solution? = " << _MeanTemperature << " MeV " << G4endl;
    G4Solver<G4StatMFMacroTemperature> * theSolverBrent = new G4Solver<G4StatMFMacroTemperature>(200,1.e-3);
    theSolverBrent->SetIntervalLimits(Ta,Tb);
    if (!theSolverBrent->Brent(*this)){
      G4cerr <<"G4StatMFMacroTemperature, Brent method failed:"<<" Ta="<<Ta<<" Tb="<<Tb<< G4endl;
      G4cerr <<"G4StatMFMacroTemperature, Brent method failed:"<<" fTa="<<fTa<<" fTb="<<fTb<< G4endl; 
	throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::CalcTemperature: I couldn't find the root with any method.");
    }

    _MeanTemperature = theSolverBrent->GetRoot();
    FunctionValureAtRoot =  this->operator()(_MeanTemperature);
    delete theSolverBrent;

     if (_MeanTemperature < 1. || _MeanTemperature > 50. || std::abs(FunctionValureAtRoot) > 5.e-2) {
    G4cout << "Brent method failed; function = " << FunctionValureAtRoot << " solution? = " << _MeanTemperature << " MeV " << G4endl;
	throw G4HadronicException(__FILE__, __LINE__, "G4StatMFMacroTemperature::CalcTemperature: I couldn't find the root with any method.");
     }
    }

    return _MeanTemperature;
}



G4double G4StatMFMacroTemperature::FragsExcitEnergy(const G4double T)
    // Calculates excitation energy per nucleon and summed fragment multiplicity and entropy
{

    // Model Parameters
    G4double R0 = G4StatMFParameters::Getr0()*std::pow(theA,1./3.);
    G4double R = R0*std::pow(1.0+G4StatMFParameters::GetKappaCoulomb(), 1./3.);
    G4double FreeVol = _Kappa*(4.*pi/3.)*R0*R0*R0; 
 
 
    // Calculate Chemical potentials
    CalcChemicalPotentialNu(T);


    // Average total fragment energy
    G4double AverageEnergy = 0.0;
    std::vector<G4VStatMFMacroCluster*>::iterator i;
    for (i =  _theClusters->begin(); i != _theClusters->end(); ++i) 
      {
	AverageEnergy += (*i)->GetMeanMultiplicity() * (*i)->CalcEnergy(T);
      }
    
    // Add Coulomb energy			
    AverageEnergy += (3./5.)*elm_coupling*theZ*theZ/R;		
    
    // Calculate mean entropy
    _MeanEntropy = 0.0;
    for (i = _theClusters->begin(); i != _theClusters->end(); ++i) 
      {
	_MeanEntropy += (*i)->CalcEntropy(T,FreeVol);	
      }

    // Excitation energy per nucleon
    G4double FragsExcitEnergy = AverageEnergy - _FreeInternalE0;

    return FragsExcitEnergy;

}


void G4StatMFMacroTemperature::CalcChemicalPotentialNu(const G4double T)
    // Calculates the chemical potential \nu 

{
    G4StatMFMacroChemicalPotential * theChemPot = new
	G4StatMFMacroChemicalPotential(theA,theZ,_Kappa,T,_theClusters);


    _ChemPotentialNu = theChemPot->CalcChemicalPotentialNu();
    _ChemPotentialMu = theChemPot->GetChemicalPotentialMu();
    _MeanMultiplicity = theChemPot->GetMeanMultiplicity();	
	
    delete theChemPot;
		    
    return;

}


