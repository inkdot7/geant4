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
// $Id: G4SimpleMaterialStopping.cc,v 1.4 2008-08-22 09:15:05 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

//---------------------------------------------------------------------------
//
// GEANT4 Class file
//
// Description: Data on stopping power
//
// Author:      A.Ivantchenko 07.08.2008
//
// Modifications:
//
//----------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4SimpleMaterialStopping.hh"
#include "G4EmCorrections.hh"
#include "G4LPhysicsFreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4SimpleMaterialStopping::G4SimpleMaterialStopping(G4EmCorrections* corr, G4bool splineFlag) 
{
  spline = splineFlag;
  Initialise(corr);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4SimpleMaterialStopping::~G4SimpleMaterialStopping()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double 
G4SimpleMaterialStopping::GetDEDX(G4int ionZ, G4int idxMaterial, G4double kinEnergy)
{
  G4double res = .0;
  if(ionZ < 3 || ionZ > 18 || idxMaterial < 0 || idxMaterial > 24) return res; 
  G4bool b;
  G4int idx = idxMaterial*16 + ionZ - 3;
  G4double scaledEnergy = kinEnergy/A[ionZ - 3];
  G4double emin = .025*MeV;
  if(scaledEnergy < emin) {
    res = (dedx[idx])->GetValue(emin, b)*sqrt(scaledEnergy/emin);
  } else {
    res = (dedx[idx])->GetValue(scaledEnergy, b);
  }
  return res;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double 
G4SimpleMaterialStopping::GetDEDX(G4int ionZ, const G4String& NameMaterial, 
			    G4double kinEnergy)
{
  return GetDEDX(ionZ, GetMaterialIndex(NameMaterial), kinEnergy);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int
G4SimpleMaterialStopping::GetMaterialIndex(const G4String& NameMaterial)
{
  for (G4int idx=0; idx<25; idx++){
    if(MatName[idx] == NameMaterial) return idx;
  }
  return -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double 
G4SimpleMaterialStopping::GetDensity(G4int idxMaterial)
{
  if( idxMaterial < 0 || idxMaterial > 24) return .0;
  return Density[idxMaterial];
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String G4SimpleMaterialStopping::GetMaterialName(G4int idxMaterial)
{
  G4String s = "";
  if( idxMaterial < 0 || idxMaterial > 24) return s;
  return MatName[idxMaterial];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4PhysicsVector* 
G4SimpleMaterialStopping::GetPhysicsVector(G4int ionZ, G4int idxMaterial)
{
  if(ionZ < 3 || ionZ > 18 || idxMaterial < 0 || idxMaterial > 24) return 0; 
  G4int idx = idxMaterial*16 + ionZ - 3;
  return dedx[idx];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4PhysicsVector* 
G4SimpleMaterialStopping::GetPhysicsVector(G4int ionZ, const G4String& NameMaterial)
{
  return GetPhysicsVector(ionZ, GetMaterialIndex(NameMaterial));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4SimpleMaterialStopping::Initialise(G4EmCorrections* corr)
{
G4int i, j=0;
dedx.reserve(16*25);

//..List of ions
//G4double factor = MeV*cm2/milligram;
G4double factor = 1000.*MeV/cm;
G4LPhysicsFreeVector* pv;  
G4double dens[25]={8.988E-05, 1.785E-04, 1.848, 1.9, .00125, .001429, 8.99E-04, 2.699, 2.329, .0017837, 4.508, 7.873, 8.907, 8.933, 5.323, .003743, 1.22, 1.5, 7.285, .005895, 19.254, 21.45, 19.291, 11.343, 19.05};
G4int Z_Ion[16] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};
G4double A_Ion[16] = {6.941,9.0122,1.811,12.011,14.007,15.999,18.998,2.180,22.990,24.305,26.982,28.086,3.974,32.065,35.453,39.948};
G4double AA_Ion[16] = {7,9,11,12,14,16,19,20,23,24,27,28,31,32,35,40};
G4String NameMaterial[25]={"G4_H", "G4_He", "G4_Be", "G4_C", "G4_N", "G4_O", "G4_Ne", "G4_Al", "G4_Si", "G4_Ar", "G4_Ti", "G4_Fe", "G4_Ni", "G4_Cu", "G4_Ge", "G4_Kr", "G4_Mo", "G4_Ag", "G4_Sn", "G4_Xe", "G4_W", "G4_Pt", "G4_Au", "G4_Pb", "G4_U"};

for(i=0; i<16; i++) {
  Z[i] = Z_Ion[i];
  A[i] = A_Ion[i];
  AA[i] = AA_Ion[i];
}
for(i=0;i<25;i++){
MatName[i]=NameMaterial[i];
Density[i]=dens[i]*gram/cm3;
}

G4double E[31] = {.025,.03,.04,.05,.06,.07,.08,.09,.1,.15,.2,.25,.3,.4,.5,.6,.7,.8,.9,1,1.5,2,2.5,3,4,5,6,7,8,9,10};
for(i=0;i<31;i++){E[i] *=MeV;}

// G4double (Z_NameMaterial)_(Z_Ion)[31]=
G4double H_3[31]={8.31,8.919,9.838,1.46,1.87,11.14,11.3,11.4,11.43,11.21,1.73,1.21,9.696,8.773,7.989,7.326,6.761,6.275,5.853,5.485,4.175,3.377,2.843,2.46,1.948,1.62,1.393,1.225,1.095,.9921,.9082};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_3[i]*dens[j]*factor);}
G4double H_4[31]={1.5,11.36,12.72,13.72,14.44,14.96,15.33,15.58,15.76,15.9,15.53,15.01,14.46,13.38,12.41,11.55,1.79,1.12,9.533,9.006,7.054,5.801,4.933,4.297,3.427,2.862,2.464,2.169,1.941,1.759,1.61};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_4[i]*dens[j]*factor);}
G4double H_5[31]={12.19,13.29,15.12,16.52,17.59,18.4,19.01,19.47,19.81,2.46,2.31,19.88,19.34,18.2,17.1,16.1,15.2,14.38,13.65,12.98,1.42,8.704,7.478,6.56,5.278,4.428,3.823,3.371,3.019,2.738,2.509};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_5[i]*dens[j]*factor);}
G4double H_6[31]={13.42,14.75,17,18.81,2.25,21.38,22.27,22.96,23.5,24.79,24.94,24.64,24.17,23.04,21.88,2.78,19.77,18.84,17.98,17.2,14.1,11.95,1.37,9.165,7.449,6.286,5.448,4.815,4.321,3.923,3.597};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_6[i]*dens[j]*factor);}
G4double H_7[31]={14.65,16.14,18.73,2.87,22.62,24.05,25.2,26.14,26.89,28.88,29.39,29.29,28.92,27.86,26.68,25.53,24.44,23.42,22.47,21.59,18.02,15.47,13.55,12.06,9.897,8.406,7.316,6.486,5.831,5.303,4.867};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_7[i]*dens[j]*factor);}
G4double H_8[31]={15.64,17.28,2.18,22.63,24.7,26.42,27.86,29.04,3.01,32.8,33.74,33.89,33.67,32.74,31.6,3.43,29.28,28.2,27.17,26.21,22.22,19.28,17.02,15.25,12.63,1.79,9.426,8.38,7.55,6.877,6.319};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_8[i]*dens[j]*factor);}
G4double H_9[31]={16.24,18,21.15,23.88,26.22,28.23,29.92,31.35,32.54,36.14,37.55,37.97,37.91,37.16,36.08,34.91,33.75,32.63,31.55,3.54,26.25,23.01,2.48,18.46,15.44,13.29,11.67,1.42,9.415,8.595,7.913};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_9[i]*dens[j]*factor);}
G4double H_10[31]={16.75,18.6,21.96,24.92,27.51,29.77,31.71,33.37,34.79,39.25,41.16,41.89,42.03,41.48,4.49,39.36,38.19,37.05,35.94,34.89,3.34,26.83,24.06,21.82,18.41,15.94,14.08,12.61,11.43,1.46,9.649};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_10[i]*dens[j]*factor);}
G4double H_11[31]={17.23,19.15,22.67,25.81,28.63,31.11,33.29,35.19,36.84,42.23,44.76,45.89,46.29,46.05,45.2,44.14,43,41.85,4.73,39.64,34.85,31.06,28.02,25.53,21.7,18.89,16.74,15.04,13.66,12.53,11.57};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_11[i]*dens[j]*factor);}
G4double H_12[31]={18,19.94,23.52,26.77,29.72,32.37,34.73,36.82,38.65,44.89,48.02,49.56,5.24,5.33,49.67,48.7,47.6,46.47,45.34,44.23,39.25,35.23,31.96,29.25,25.04,21.91,19.5,17.58,16.01,14.71,13.61};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_12[i]*dens[j]*factor);}
G4double H_13[31]={18.67,2.65,24.32,27.67,3.74,33.53,36.06,38.32,4.33,47.38,51.11,53.06,54.04,54.51,54.05,53.2,52.17,51.07,49.94,48.83,43.71,39.47,36,33.09,28.51,25.07,22.39,2.25,18.49,17.02,15.77};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_13[i]*dens[j]*factor);}
G4double H_14[31]={19.33,21.35,25.1,28.53,31.69,34.6,37.26,39.67,41.83,49.62,53.93,56.29,57.56,58.4,58.15,57.43,56.47,55.41,54.31,53.2,47.99,43.59,39.93,36.85,31.95,28.23,25.32,22.96,21.02,19.39,18.01};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_14[i]*dens[j]*factor);}
G4double H_15[31]={2.27,22.34,26.16,29.67,32.92,35.93,38.7,41.24,43.54,52.03,56.89,59.67,61.24,62.49,62.48,61.9,61.04,6.04,58.96,57.86,52.58,48.02,44.17,4.89,35.65,31.64,28.46,25.88,23.75,21.95,2.41};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_15[i]*dens[j]*factor);}
G4double H_16[31]={2.94,23.07,26.99,3.59,33.94,37.06,39.96,42.63,45.08,54.29,59.75,62.97,64.88,66.56,66.82,66.41,65.66,64.72,63.69,62.62,57.29,52.58,48.55,45.1,39.52,35.2,31.76,28.96,26.62,24.64,22.95};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_16[i]*dens[j]*factor);}
G4double H_17[31]={21.89,24.05,28,31.64,35.03,38.21,41.18,43.94,46.49,56.31,62.31,65.95,68.16,7.25,7.76,7.52,69.87,69.01,68.02,66.97,61.64,56.83,52.67,49.08,43.22,38.65,34.98,31.98,29.46,27.33,25.49};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_17[i]*dens[j]*factor);}
G4double H_18[31]={22.3,24.5,28.52,32.2,35.64,38.87,41.91,44.76,47.41,57.82,64.39,68.46,71,73.53,74.31,74.25,73.72,72.94,72.01,71,65.7,6.83,56.57,52.86,46.78,41.99,38.13,34.95,32.28,30,28.04};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],H_18[i]*dens[j]*factor);}
j++;

G4double He_3[31]={2.636,2.851,3.202,3.47,3.676,3.832,3.948,4.035,4.097,4.194,4.123,3.996,3.85,3.558,3.29,3.052,2.842,2.656,2.492,2.346,1.812,1.476,1.247,1.082,.8605,.7182,.6189,.5455,.4889,.4437,.4068};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_3[i]*dens[j]*factor);}
G4double He_4[31]={3.352,3.645,4.14,4.539,4.862,5.122,5.33,5.495,5.626,5.943,5.972,5.886,5.753,5.439,5.122,4.824,4.55,4.302,4.077,3.871,3.08,2.553,2.179,1.903,1.522,1.274,1.098,.9684,.8679,.7877,.7222};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_4[i]*dens[j]*factor);}
G4double He_5[31]={3.917,4.284,4.921,5.452,5.897,6.268,6.577,6.832,7.043,7.634,7.806,7.794,7.697,7.402,7.067,6.735,6.419,6.123,5.848,5.592,4.569,3.85,3.323,2.923,2.358,1.981,1.712,1.511,1.354,1.229,1.127};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_5[i]*dens[j]*factor);}
G4double He_6[31]={4.345,4.779,5.548,6.207,6.774,7.26,7.675,8.027,8.325,9.235,9.584,9.671,9.631,9.382,9.051,8.703,8.36,8.029,7.717,7.423,6.202,5.306,4.629,4.104,3.344,2.826,2.45,2.166,1.944,1.766,1.62};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_6[i]*dens[j]*factor);}
G4double He_7[31]={4.78,5.263,6.129,6.889,7.557,8.144,8.656,9.102,9.487,1.73,11.29,11.5,11.54,11.36,11.05,1.7,1.35,9.995,9.655,9.331,7.944,6.887,6.069,5.42,4.463,3.795,3.304,2.929,2.634,2.395,2.199};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_7[i]*dens[j]*factor);}
G4double He_8[31]={5.133,5.662,6.619,7.473,8.238,8.921,9.528,1.07,1.54,12.15,12.93,13.29,13.42,13.36,13.1,12.76,12.4,12.04,11.68,11.34,9.814,8.61,7.654,6.882,5.721,4.894,4.278,3.802,3.425,3.119,2.865};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_8[i]*dens[j]*factor);}
G4double He_9[31]={5.37,5.934,6.966,7.897,8.742,9.509,1.2,1.82,11.38,13.33,14.35,14.86,15.09,15.13,14.92,14.61,14.26,13.9,13.53,13.17,11.55,1.24,9.184,8.317,6.989,6.025,5.297,4.729,4.274,3.901,3.591};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_9[i]*dens[j]*factor);}
G4double He_10[31]={5.57,6.164,7.257,8.253,9.168,1.01,1.78,11.47,12.11,14.4,15.67,16.35,16.69,16.86,16.71,16.43,16.09,15.73,15.36,14.99,13.3,11.9,1.75,9.798,8.315,7.221,6.383,5.722,5.189,4.749,4.381};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_10[i]*dens[j]*factor);}
G4double He_11[31]={5.709,6.323,7.464,8.515,9.493,1.4,11.24,12.02,12.73,15.4,16.95,17.83,18.31,18.66,18.6,18.38,18.07,17.72,17.36,16.99,15.25,13.76,12.52,11.47,9.812,8.571,7.609,6.843,6.22,5.704,5.269};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_11[i]*dens[j]*factor);}
G4double He_12[31]={5.934,6.556,7.715,8.793,9.807,1.76,11.65,12.49,13.26,16.25,18.08,19.16,19.79,2.33,2.38,2.22,19.94,19.62,19.27,18.9,17.13,15.57,14.25,13.12,11.32,9.946,8.87,8.005,7.297,6.706,6.206};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_12[i]*dens[j]*factor);}
G4double He_13[31]={6.134,6.771,7.956,9.062,1.11,11.1,12.04,12.92,13.74,17.03,19.13,2.42,21.21,21.95,22.13,22.04,21.82,21.52,21.19,2.83,19.04,17.43,16.03,14.83,12.89,11.38,1.2,9.233,8.438,7.771,7.205};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_13[i]*dens[j]*factor);}
G4double He_14[31]={6.339,6.99,8.198,9.326,1.4,11.42,12.39,13.31,14.18,17.73,2.07,21.57,22.51,23.46,23.76,23.75,23.57,23.31,22.99,22.65,2.85,19.2,17.75,16.48,14.41,12.8,11.51,1.46,9.59,8.853,8.225};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_14[i]*dens[j]*factor);}
G4double He_15[31]={6.634,7.302,8.541,9.696,1.79,11.84,12.85,13.81,14.72,18.51,21.1,22.8,23.91,25.08,25.52,25.6,25.49,25.27,24.98,24.66,22.88,21.18,19.66,18.33,16.12,14.38,12.98,11.82,1.86,1.05,9.347};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_15[i]*dens[j]*factor);}
G4double He_16[31]={6.833,7.524,8.803,9.993,11.12,12.21,13.25,14.24,15.2,19.23,22.07,23.99,25.27,26.69,27.28,27.46,27.42,27.25,27,26.7,24.96,23.23,21.66,2.26,17.91,16.05,14.53,13.27,12.22,11.32,1.54};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_16[i]*dens[j]*factor);}
G4double He_17[31]={7.144,7.843,9.139,1.34,11.49,12.59,13.65,14.68,15.66,19.88,22.95,25.06,26.5,28.14,28.87,29.15,29.17,29.05,28.83,28.55,26.84,25.09,23.48,22.03,19.58,17.61,16,14.66,13.52,12.55,11.72};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_17[i]*dens[j]*factor);}
G4double He_18[31]={7.268,7.978,9.289,1.51,11.66,12.78,13.85,14.89,15.9,2.3,23.58,25.89,27.49,29.36,3.24,3.62,3.71,3.63,3.44,3.19,28.52,26.76,25.12,23.64,21.12,19.07,17.38,15.97,14.77,13.75,12.85};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],He_18[i]*dens[j]*factor);}
j++;

G4double Be_3[31]={2.148,2.307,2.562,2.753,2.897,3.003,3.08,3.132,3.166,3.193,3.127,3.033,2.931,2.727,2.538,2.368,2.217,2.082,1.961,1.854,1.451,1.192,1.013,.8818,.7042,.5894,.5089,.4493,.4032,.3664,.3363};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_3[i]*dens[j]*factor);}
G4double Be_4[31]={2.702,2.928,3.303,3.599,3.833,4.017,4.162,4.273,4.357,4.533,4.535,4.474,4.386,4.177,3.961,3.754,3.561,3.382,3.218,3.067,2.476,2.07,1.777,1.557,1.251,1.049,.9061,.7996,.7172,.6515,.5978};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_4[i]*dens[j]*factor);}
G4double Be_5[31]={3.141,3.432,3.925,4.326,4.654,4.922,5.141,5.32,5.463,5.831,5.93,5.924,5.868,5.687,5.471,5.247,5.029,4.82,4.624,4.44,3.682,3.131,2.72,2.402,1.947,1.639,1.418,1.252,1.123,1.02,.9352};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_5[i]*dens[j]*factor);}
G4double Be_6[31]={3.484,3.837,4.444,4.949,5.372,5.727,6.025,6.275,6.483,7.078,7.297,7.363,7.353,7.22,7.02,6.796,6.566,6.339,6.12,5.911,5.014,4.332,3.805,3.388,2.775,2.35,2.039,1.803,1.619,1.47,1.349};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_6[i]*dens[j]*factor);}
G4double Be_7[31]={3.795,4.198,4.903,5.499,6.008,6.445,6.819,7.14,7.413,8.257,8.619,8.774,8.824,8.761,8.595,8.383,8.153,7.917,7.684,7.457,6.449,5.649,5.011,4.497,3.723,3.173,2.764,2.451,2.203,2.003,1.838};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_7[i]*dens[j]*factor);}
G4double Be_8[31]={4.05,4.499,5.294,5.978,6.57,7.085,7.534,7.924,8.263,9.368,9.89,1.15,1.27,1.3,1.18,9.998,9.78,9.546,9.308,9.071,7.978,7.075,6.336,5.727,4.791,4.109,3.597,3.197,2.879,2.62,2.406};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_8[i]*dens[j]*factor);}
G4double Be_9[31]={4.249,4.737,5.609,6.369,7.035,7.621,8.137,8.591,8.991,1.35,11.03,11.39,11.59,11.7,11.64,11.48,11.28,11.05,1.81,1.57,9.42,8.439,7.621,6.937,5.866,5.072,4.465,3.987,3.602,3.286,3.023};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_9[i]*dens[j]*factor);}
G4double Be_10[31]={4.423,4.942,5.88,6.708,7.442,8.094,8.674,9.189,9.647,11.26,12.11,12.58,12.85,13.07,13.07,12.94,12.76,12.55,12.31,12.07,1.87,9.823,8.936,8.184,6.988,6.087,5.389,4.833,4.381,4.008,3.695};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_10[i]*dens[j]*factor);}
G4double Be_11[31]={4.542,5.085,6.08,6.967,7.763,8.478,9.12,9.697,1.22,12.09,13.14,13.75,14.11,14.46,14.53,14.46,14.31,14.11,13.89,13.65,12.43,11.33,1.38,9.559,8.236,7.222,6.425,5.784,5.259,4.822,4.452};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_11[i]*dens[j]*factor);}
G4double Be_12[31]={4.691,5.249,6.282,7.213,8.055,8.819,9.511,1.14,1.71,12.83,14.06,14.8,15.27,15.75,15.91,15.89,15.77,15.6,15.39,15.16,13.93,12.79,11.79,1.91,9.483,8.37,7.485,6.765,6.171,5.673,5.249};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_12[i]*dens[j]*factor);}
G4double Be_13[31]={4.831,5.402,6.469,7.44,8.325,9.133,9.871,1.54,11.16,13.5,14.93,15.81,16.38,17.01,17.26,17.31,17.24,17.09,16.91,16.69,15.47,14.3,13.25,12.32,1.79,9.579,8.605,7.807,7.142,6.582,6.103};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_13[i]*dens[j]*factor);}
G4double Be_14[31]={4.976,5.558,6.651,7.655,8.577,9.423,1.2,1.92,11.57,14.12,15.73,16.74,17.42,18.19,18.54,18.66,18.63,18.52,18.35,18.15,16.95,15.75,14.66,13.69,12.07,1.77,9.722,8.852,8.123,7.505,6.974};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_14[i]*dens[j]*factor);}
G4double Be_15[31]={5.153,5.747,6.866,7.902,8.859,9.743,1.56,11.31,12.01,14.76,16.55,17.72,18.5,19.44,19.9,2.1,2.14,2.08,19.94,19.77,18.62,17.4,16.27,15.26,13.53,12.13,1.99,1.04,9.232,8.545,7.953};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_15[i]*dens[j]*factor);}
G4double Be_16[31]={5.3,5.907,7.053,8.12,9.113,1.03,1.89,11.68,12.42,15.37,17.35,18.66,19.56,2.67,21.25,21.54,21.65,21.63,21.54,21.39,2.3,19.09,17.93,16.87,15.05,13.56,12.33,11.29,1.41,9.655,9.001};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_16[i]*dens[j]*factor);}
G4double Be_17[31]={5.5,6.113,7.275,8.362,9.379,1.33,11.21,12.04,12.81,15.93,18.08,19.53,2.55,21.82,22.51,22.89,23.05,23.08,23.02,22.9,21.87,2.65,19.47,18.38,16.49,14.92,13.6,12.5,11.55,1.73,1.02};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_17[i]*dens[j]*factor);}
G4double Be_18[31]={5.619,6.238,7.409,8.508,9.542,1.51,11.42,12.28,13.07,16.35,18.66,2.25,21.37,22.8,23.61,24.07,24.29,24.37,24.34,24.25,23.27,22.05,2.85,19.74,17.78,16.14,14.77,13.61,12.61,11.75,1.99};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Be_18[i]*dens[j]*factor);}
j++;

G4double C_3[31]={2.043,2.2,2.457,2.658,2.815,2.939,3.035,3.109,3.162,3.238,3.178,3.082,2.978,2.774,2.587,2.419,2.27,2.137,2.018,1.911,1.508,1.246,1.063,.9292,.7457,.6261,.5418,.4792,.4306,.3918,.3599};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_3[i]*dens[j]*factor);}
G4double C_4[31]={2.54,2.761,3.135,3.441,3.694,3.902,4.072,4.21,4.321,4.588,4.608,4.546,4.455,4.245,4.032,3.83,3.641,3.467,3.306,3.159,2.572,2.163,1.866,1.641,1.325,1.115,.9653,.8533,.7664,.6969,.64};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_4[i]*dens[j]*factor);}
G4double C_5[31]={2.919,3.203,3.692,4.102,4.449,4.743,4.992,5.202,5.378,5.882,6.02,6.019,5.96,5.776,5.562,5.346,5.136,4.935,4.745,4.566,3.821,3.272,2.856,2.532,2.063,1.743,1.512,1.337,1.201,1.092,1.002};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_5[i]*dens[j]*factor);}
G4double C_6[31]={3.206,3.548,4.148,4.659,5.099,5.479,5.809,6.094,6.34,7.113,7.401,7.482,7.471,7.335,7.139,6.923,6.703,6.487,6.278,6.077,5.205,4.529,3.998,3.574,2.944,2.502,2.177,1.929,1.734,1.576,1.447};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_6[i]*dens[j]*factor);}
G4double C_7[31]={3.445,3.836,4.53,5.131,5.657,6.119,6.527,6.885,7.2,8.259,8.728,8.914,8.969,8.905,8.742,8.541,8.324,8.103,7.884,7.668,6.698,5.91,5.273,4.751,3.957,3.386,2.958,2.627,2.364,2.152,1.975};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_7[i]*dens[j]*factor);}
G4double C_8[31]={3.638,4.069,4.848,5.533,6.141,6.682,7.165,7.595,7.979,9.328,9.997,1.31,1.44,1.47,1.36,1.18,9.98,9.764,9.543,9.321,8.282,7.401,6.666,6.053,5.096,4.39,3.853,3.432,3.095,2.819,2.59};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_8[i]*dens[j]*factor);}
G4double C_9[31]={3.792,4.257,5.106,5.864,6.544,7.156,7.707,8.203,8.65,1.28,11.15,11.59,11.8,11.93,11.87,11.72,11.54,11.33,11.11,1.89,9.8,8.848,8.037,7.348,6.254,5.431,4.794,4.289,3.88,3.543,3.261};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_9[i]*dens[j]*factor);}
G4double C_10[31]={3.927,4.419,5.328,6.149,6.893,7.57,8.185,8.744,9.252,11.15,12.23,12.81,13.12,13.35,13.35,13.24,13.08,12.88,12.67,12.45,11.33,1.32,9.442,8.686,7.464,6.529,5.796,5.208,4.728,4.329,3.993};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_10[i]*dens[j]*factor);}
G4double C_11[31]={4.019,4.53,5.486,6.362,7.165,7.903,8.579,9.2,9.768,11.94,13.26,14,14.42,14.79,14.86,14.8,14.67,14.49,14.29,14.08,12.95,11.89,1.96,1.14,8.79,7.742,6.908,6.232,5.675,5.208,4.813};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_11[i]*dens[j]*factor);}
G4double C_12[31]={4.132,4.653,5.638,6.555,7.407,8.194,8.922,9.593,1.21,12.64,14.18,15.08,15.62,16.13,16.29,16.28,16.18,16.03,15.84,15.64,14.51,13.42,12.43,11.56,1.11,8.965,8.041,7.285,6.656,6.126,5.674};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_12[i]*dens[j]*factor);}
G4double C_13[31]={4.239,4.77,5.781,6.732,7.623,8.455,9.228,9.947,1.61,13.28,15.03,16.11,16.77,17.44,17.7,17.75,17.69,17.57,17.41,17.22,16.11,14.99,13.97,13.05,11.5,1.25,9.242,8.412,7.705,7.108,6.597};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_13[i]*dens[j]*factor);}
G4double C_14[31]={4.352,4.888,5.918,6.895,7.819,8.689,9.503,1.26,1.97,13.85,15.81,17.06,17.84,18.68,19.04,19.16,19.15,19.06,18.91,18.74,17.66,16.53,15.47,14.51,12.87,11.54,1.45,9.535,8.767,8.111,7.545};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_14[i]*dens[j]*factor);}
G4double C_15[31]={4.48,5.022,6.068,7.07,8.023,8.927,9.779,1.58,11.33,14.43,16.59,18.01,18.94,19.96,2.44,2.64,2.69,2.65,2.54,2.39,19.37,18.24,17.16,16.16,14.43,13,11.82,1.82,9.972,9.246,8.616};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_15[i]*dens[j]*factor);}
G4double C_16[31]={4.597,5.146,6.21,7.235,8.216,9.151,1.04,1.88,11.67,14.97,17.33,18.94,20,21.22,21.83,22.12,22.23,22.24,22.17,22.06,21.11,19.99,18.89,17.86,16.05,14.53,13.26,12.18,11.25,1.45,9.757};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_16[i]*dens[j]*factor);}
G4double C_17[31]={4.745,5.296,6.37,7.409,8.41,9.37,1.29,11.16,11.98,15.46,18.01,19.79,21,22.41,23.14,23.52,23.7,23.75,23.72,23.63,22.76,21.65,2.54,19.48,17.59,16,14.65,13.49,12.5,11.64,1.88};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_17[i]*dens[j]*factor);}
G4double C_18[31]={4.847,5.401,6.479,7.526,8.54,9.518,1.46,11.35,12.21,15.85,18.57,2.52,21.86,23.47,24.32,24.78,25.02,25.12,25.12,25.06,24.25,23.16,22.02,2.94,19,17.34,15.92,14.71,13.66,12.75,11.95};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],C_18[i]*dens[j]*factor);}
j++;

G4double N_3[31]={2.13,2.311,2.613,2.849,3.03,3.166,3.264,3.333,3.381,3.429,3.351,3.239,3.12,2.891,2.685,2.503,2.343,2.201,2.075,1.963,1.544,1.274,1.086,.9483,.7603,.638,.5519,.4879,.4382,.3986,.3661};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_3[i]*dens[j]*factor);}
G4double N_4[31]={2.65,2.892,3.309,3.658,3.947,4.181,4.367,4.512,4.622,4.861,4.862,4.781,4.671,4.427,4.186,3.963,3.757,3.57,3.399,3.242,2.629,2.208,1.903,1.673,1.35,1.136,.9828,.8685,.7799,.709,.6509};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_4[i]*dens[j]*factor);}
G4double N_5[31]={3.07,3.37,3.894,4.342,4.728,5.057,5.333,5.56,5.745,6.232,6.351,6.331,6.251,6.026,5.778,5.534,5.301,5.08,4.875,4.684,3.903,3.335,2.909,2.578,2.1,1.774,1.538,1.36,1.222,1.11,1.019};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_5[i]*dens[j]*factor);}
G4double N_6[31]={3.409,3.764,4.389,4.93,5.404,5.821,6.183,6.494,6.756,7.532,7.805,7.868,7.836,7.656,7.418,7.168,6.92,6.681,6.452,6.235,5.315,4.615,4.07,3.637,2.995,2.545,2.215,1.962,1.763,1.603,1.471};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_6[i]*dens[j]*factor);}
G4double N_7[31]={3.704,4.106,4.82,5.441,5.993,6.487,6.927,7.315,7.654,8.74,9.198,9.369,9.404,9.295,9.088,8.847,8.597,8.347,8.105,7.87,6.839,6.021,5.367,4.834,4.024,3.443,3.008,2.672,2.405,2.188,2.009};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_7[i]*dens[j]*factor);}
G4double N_8[31]={3.958,4.401,5.196,5.894,6.518,7.082,7.593,8.053,8.463,9.864,1.53,1.83,1.94,1.93,1.77,1.55,1.31,1.06,9.811,9.566,8.454,7.535,6.78,6.153,5.179,4.463,3.917,3.49,3.147,2.867,2.634};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_8[i]*dens[j]*factor);}
G4double N_9[31]={4.177,4.656,5.522,6.288,6.976,7.602,8.173,8.695,9.168,1.87,11.75,12.18,12.39,12.47,12.36,12.17,11.94,11.69,11.44,11.19,1.02,9.022,8.185,7.479,6.362,5.524,4.877,4.363,3.947,3.605,3.318};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_9[i]*dens[j]*factor);}
G4double N_10[31]={4.375,4.884,5.813,6.641,7.388,8.071,8.698,9.275,9.805,11.79,12.89,13.48,13.78,13.98,13.93,13.77,13.56,13.32,13.07,12.82,11.6,1.54,9.628,8.851,7.601,6.648,5.901,5.303,4.813,4.407,4.065};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_10[i]*dens[j]*factor);}
G4double N_11[31]={4.532,5.065,6.047,6.932,7.735,8.47,9.15,9.78,1.36,12.63,13.97,14.72,15.14,15.48,15.51,15.4,15.21,14.99,14.75,14.51,13.27,12.15,11.17,1.33,8.95,7.88,7.031,6.342,5.775,5.3,4.898};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_11[i]*dens[j]*factor);}
G4double N_12[31]={4.702,5.251,6.272,7.203,8.054,8.835,9.559,1.23,1.86,13.38,14.95,15.87,16.41,16.9,17.01,16.95,16.8,16.6,16.37,16.13,14.88,13.72,12.69,11.79,1.3,9.125,8.183,7.413,6.773,6.233,5.773};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_12[i]*dens[j]*factor);}
G4double N_13[31]={4.858,5.424,6.48,7.449,8.342,9.168,9.934,1.65,11.32,14.06,15.86,16.96,17.63,18.29,18.5,18.5,18.39,18.22,18.01,17.78,16.53,15.33,14.26,13.31,11.71,1.44,9.405,8.552,7.838,7.232,6.713};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_13[i]*dens[j]*factor);}
G4double N_14[31]={5.011,5.59,6.676,7.68,8.611,9.475,1.28,11.03,11.74,14.67,16.69,17.97,18.77,19.61,19.92,19.99,19.92,19.78,19.59,19.37,18.14,16.92,15.8,14.8,13.11,11.75,1.64,9.706,8.923,8.255,7.679};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_14[i]*dens[j]*factor);}
G4double N_15[31]={5.17,5.761,6.874,7.91,8.877,9.779,1.62,11.41,12.15,15.28,17.5,18.97,19.92,2.95,21.39,21.54,21.54,21.44,21.28,21.08,19.9,18.67,17.52,16.48,14.7,13.24,12.03,11.01,1.15,9.409,8.768};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_15[i]*dens[j]*factor);}
G4double N_16[31]={5.313,5.919,7.061,8.131,9.133,1.07,1.95,11.77,12.55,15.85,18.28,19.93,21.03,22.27,22.84,23.08,23.14,23.09,22.97,22.8,21.68,2.45,19.29,18.21,16.34,14.78,13.48,12.39,11.45,1.63,9.926};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_16[i]*dens[j]*factor);}
G4double N_17[31]={5.481,6.098,7.262,8.355,9.386,1.35,11.26,12.12,12.92,16.38,19,2.83,22.08,23.52,24.22,24.55,24.67,24.67,24.58,24.44,23.38,22.16,2.98,19.87,17.92,16.29,14.91,13.73,12.72,11.84,11.08};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_17[i]*dens[j]*factor);}
G4double N_18[31]={5.604,6.233,7.416,8.529,9.581,1.57,11.51,12.39,13.22,16.82,19.6,21.61,23,24.65,25.48,25.9,26.08,26.12,26.07,25.96,24.95,23.73,22.53,21.39,19.38,17.67,16.23,14.99,13.92,12.99,12.17};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],N_18[i]*dens[j]*factor);}
j++;

G4double O_3[31]={1.865,2.032,2.312,2.534,2.709,2.843,2.944,3.019,3.072,3.153,3.1,3.007,2.904,2.702,2.517,2.353,2.207,2.077,1.962,1.858,1.468,1.215,1.038,.9083,.7302,.6139,.5318,.4706,.4232,.3852,.354};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_3[i]*dens[j]*factor);}
G4double O_4[31]={2.293,2.519,2.909,3.236,3.509,3.735,3.918,4.065,4.18,4.458,4.489,4.433,4.342,4.132,3.919,3.719,3.534,3.364,3.207,3.064,2.497,2.105,1.819,1.602,1.296,1.092,.9469,.8378,.753,.6851,.6295};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_4[i]*dens[j]*factor);}
G4double O_5[31]={2.624,2.903,3.393,3.813,4.176,4.49,4.757,4.982,5.169,5.696,5.853,5.861,5.805,5.618,5.403,5.187,4.978,4.781,4.595,4.421,3.704,3.177,2.778,2.467,2.016,1.706,1.482,1.312,1.18,1.073,.9855};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_5[i]*dens[j]*factor);}
G4double O_6[31]={2.883,3.21,3.792,4.3,4.747,5.143,5.49,5.792,6.051,6.861,7.178,7.275,7.27,7.133,6.932,6.714,6.495,6.282,6.076,5.88,5.04,4.392,3.885,3.479,2.874,2.448,2.134,1.893,1.703,1.55,1.423};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_6[i]*dens[j]*factor);}
G4double O_7[31]={3.103,3.469,4.13,4.714,5.236,5.704,6.124,6.497,6.827,7.932,8.439,8.65,8.715,8.653,8.486,8.281,8.063,7.843,7.627,7.417,6.482,5.729,5.121,4.623,3.862,3.312,2.899,2.578,2.323,2.115,1.944};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_7[i]*dens[j]*factor);}
G4double O_8[31]={3.292,3.69,4.419,5.072,5.662,6.198,6.684,7.124,7.52,8.922,9.637,9.981,1.13,1.17,1.05,9.869,9.663,9.446,9.227,9.009,8.006,7.164,6.467,5.883,4.969,4.292,3.775,3.367,3.04,2.772,2.549};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_8[i]*dens[j]*factor);}
G4double O_9[31]={3.458,3.881,4.667,5.381,6.03,6.623,7.168,7.666,8.121,9.803,1.73,11.21,11.46,11.6,11.54,11.38,11.19,1.98,1.76,1.54,9.49,8.58,7.808,7.152,6.106,5.316,4.702,4.213,3.815,3.487,3.212};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_9[i]*dens[j]*factor);}
G4double O_10[31]={3.612,4.056,4.889,5.655,6.357,7.003,7.601,8.152,8.661,1.61,11.75,12.39,12.74,13,13,12.88,12.71,12.51,12.3,12.08,1.99,1.02,9.187,8.466,7.298,6.4,5.692,5.122,4.655,4.266,3.937};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_10[i]*dens[j]*factor);}
G4double O_11[31]={3.734,4.194,5.066,5.876,6.626,7.321,7.967,8.569,9.128,11.33,12.71,13.52,13.98,14.39,14.47,14.4,14.26,14.08,13.88,13.66,12.56,11.55,1.66,9.877,8.591,7.584,6.78,6.126,5.584,5.13,4.744};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_11[i]*dens[j]*factor);}
G4double O_12[31]={3.869,4.34,5.238,6.083,6.872,7.608,8.296,8.939,9.541,11.98,13.57,14.55,15.14,15.71,15.88,15.86,15.75,15.59,15.4,15.19,14.09,13.04,12.1,11.27,9.882,8.78,7.89,7.158,6.548,6.032,5.591};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_12[i]*dens[j]*factor);}
G4double O_13[31]={3.995,4.477,5.398,6.269,7.09,7.864,8.591,9.273,9.915,12.56,14.37,15.52,16.25,16.99,17.26,17.31,17.24,17.11,16.94,16.74,15.64,14.57,13.59,12.72,11.23,1.04,9.066,8.257,7.577,6.998,6.501};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_13[i]*dens[j]*factor);}
G4double O_14[31]={4.121,4.612,5.552,6.446,7.295,8.1,8.86,9.576,1.25,13.09,15.09,16.42,17.29,18.21,18.58,18.7,18.68,18.58,18.43,18.24,17.17,16.07,15.06,14.15,12.58,11.3,1.25,9.372,8.626,7.989,7.437};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_14[i]*dens[j]*factor);}
G4double O_15[31]={4.248,4.749,5.704,6.618,7.493,8.327,9.118,9.867,1.58,13.59,15.79,17.3,18.31,19.44,19.94,2.15,2.19,2.13,2.01,19.85,18.83,17.73,16.7,15.75,14.1,12.73,11.59,1.63,9.812,9.106,8.493};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_15[i]*dens[j]*factor);}
G4double O_16[31]={4.365,4.877,5.853,6.788,7.688,8.549,9.37,1.15,1.89,14.07,16.46,18.15,19.31,2.64,21.28,21.58,21.68,21.67,21.59,21.46,2.51,19.42,18.37,17.39,15.66,14.21,12.99,11.95,11.06,1.29,9.614};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_16[i]*dens[j]*factor);}
G4double O_17[31]={4.501,5.022,6.013,6.964,7.882,8.766,9.61,1.42,11.18,14.52,17.07,18.93,2.23,21.78,22.56,22.95,23.12,23.16,23.11,23.01,22.12,21.04,19.98,18.97,17.18,15.66,14.37,13.26,12.3,11.47,1.73};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_17[i]*dens[j]*factor);}
G4double O_18[31]={4.604,5.133,6.139,7.102,8.034,8.934,9.799,1.63,11.42,14.88,17.59,19.61,21.06,22.82,23.73,24.21,24.44,24.53,24.52,24.44,23.61,22.54,21.46,2.43,18.58,17,15.64,14.47,13.46,12.58,11.8};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],O_18[i]*dens[j]*factor);}
j++;

G4double Ne_3[31]={1.497,1.622,1.831,1.999,2.136,2.25,2.343,2.42,2.481,2.631,2.638,2.589,2.519,2.364,2.215,2.079,1.957,1.848,1.749,1.66,1.322,1.1,.9434,.8276,.6681,.5634,.4893,.4339,.3908,.3562,.3278};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_3[i]*dens[j]*factor);}
G4double Ne_4[31]={1.876,2.046,2.338,2.58,2.787,2.964,3.117,3.248,3.36,3.697,3.808,3.811,3.765,3.618,3.452,3.29,3.136,2.993,2.861,2.738,2.249,1.905,1.652,1.46,1.186,1.003,.8715,.7727,.6956,.6338,.583};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_4[i]*dens[j]*factor);}
G4double Ne_5[31]={2.174,2.385,2.752,3.065,3.337,3.576,3.787,3.975,4.14,4.693,4.939,5.024,5.025,4.919,4.76,4.589,4.419,4.255,4.099,3.952,3.335,2.875,2.523,2.247,1.845,1.567,1.364,1.211,1.09,.9929,.913};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_5[i]*dens[j]*factor);}
G4double Ne_6[31]={2.414,2.661,3.1,3.48,3.815,4.113,4.382,4.624,4.841,5.622,6.026,6.212,6.277,6.237,6.104,5.939,5.765,5.591,5.42,5.256,4.538,3.975,3.529,3.17,2.631,2.249,1.965,1.747,1.574,1.434,1.319};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_6[i]*dens[j]*factor);}
G4double Ne_7[31]={2.603,2.879,3.377,3.817,4.21,4.565,4.887,5.181,5.45,6.461,7.043,7.352,7.499,7.553,7.465,7.32,7.153,6.978,6.801,6.627,5.834,5.184,4.652,4.213,3.537,3.044,2.671,2.381,2.149,1.959,1.802};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_7[i]*dens[j]*factor);}
G4double Ne_8[31]={2.789,3.09,3.64,4.133,4.578,4.984,5.356,5.698,6.013,7.245,8.009,8.452,8.695,8.863,8.833,8.72,8.569,8.401,8.225,8.048,7.204,6.481,5.873,5.36,4.55,3.945,3.479,3.111,2.813,2.569,2.365};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_8[i]*dens[j]*factor);}
G4double Ne_9[31]={2.946,3.267,3.86,4.399,4.892,5.344,5.762,6.148,6.507,7.945,8.888,9.47,9.813,1.11,1.14,1.06,9.933,9.775,9.604,9.426,8.547,7.768,7.097,6.523,5.597,4.891,4.338,3.895,3.534,3.234,2.983};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_9[i]*dens[j]*factor);}
G4double Ne_10[31]={3.091,3.431,4.061,4.64,5.175,5.67,6.13,6.558,6.957,8.59,9.709,1.43,1.89,11.32,11.43,11.39,11.29,11.15,1.98,1.81,9.909,9.083,8.359,7.728,6.696,5.894,5.256,4.741,4.316,3.961,3.66};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_10[i]*dens[j]*factor);}
G4double Ne_11[31]={3.201,3.555,4.218,4.832,5.406,5.941,6.442,6.91,7.349,9.177,1.48,11.36,11.93,12.52,12.72,12.74,12.67,12.55,12.4,12.23,11.33,1.47,9.695,9.015,7.882,6.985,6.262,5.67,5.179,4.764,4.412};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_11[i]*dens[j]*factor);}
G4double Ne_12[31]={3.316,3.68,4.365,5.005,5.608,6.176,6.71,7.213,7.688,9.693,11.17,12.2,12.9,13.66,13.96,14.04,14,13.9,13.77,13.61,12.71,11.82,11.01,1.29,9.066,8.086,7.287,6.626,6.073,5.603,5.2};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_12[i]*dens[j]*factor);}
G4double Ne_13[31]={3.429,3.801,4.503,5.164,5.791,6.386,6.95,7.485,7.992,1.16,11.8,12.98,13.81,14.76,15.17,15.32,15.33,15.27,15.15,15,14.12,13.21,12.37,11.61,1.31,9.247,8.373,7.644,7.027,6.501,6.046};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_13[i]*dens[j]*factor);}
G4double Ne_14[31]={3.539,3.919,4.635,5.311,5.957,6.574,7.164,7.725,8.261,1.58,12.36,13.69,14.65,15.79,16.33,16.56,16.62,16.58,16.49,16.36,15.51,14.58,13.72,12.92,11.55,1.42,9.473,8.679,8.004,7.423,6.92};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_14[i]*dens[j]*factor);}
G4double Ne_15[31]={3.647,4.032,4.759,5.448,6.108,6.744,7.355,7.942,8.503,1.96,12.89,14.37,15.46,16.81,17.49,17.82,17.95,17.96,17.91,17.8,17,16.09,15.2,14.38,12.93,11.73,1.71,9.845,9.104,8.463,7.904};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_15[i]*dens[j]*factor);}
G4double Ne_16[31]={3.75,4.145,4.888,5.592,6.268,6.922,7.554,8.163,8.749,11.34,13.41,15.03,16.25,17.81,18.64,19.07,19.27,19.34,19.32,19.25,18.52,17.61,16.72,15.87,14.37,13.09,12,11.07,1.26,9.562,8.947};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_16[i]*dens[j]*factor);}
G4double Ne_17[31]={3.861,4.261,5.013,5.725,6.413,7.081,7.73,8.359,8.965,11.67,13.88,15.62,16.97,18.75,19.73,2.26,2.53,2.65,2.68,2.63,19.98,19.09,18.19,17.32,15.77,14.43,13.27,12.28,11.42,1.66,9.997};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_17[i]*dens[j]*factor);}
G4double Ne_18[31]={3.955,4.36,5.12,5.839,6.536,7.216,7.88,8.526,9.152,11.97,14.29,16.16,17.62,19.6,2.73,21.37,21.72,21.89,21.95,21.94,21.35,2.47,19.56,18.68,17.07,15.67,14.46,13.42,12.51,11.7,1.99};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ne_18[i]*dens[j]*factor);}
j++;

G4double Al_3[31]={1.388,1.496,1.67,1.8,1.898,1.971,2.024,2.063,2.093,2.164,2.172,2.15,2.111,2.01,1.901,1.797,1.701,1.612,1.532,1.459,1.176,.9856,.8498,.7483,.6073,.514,.4476,.3977,.3588,.3275,.3018};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_3[i]*dens[j]*factor);}
G4double Al_4[31]={1.754,1.91,2.167,2.37,2.531,2.659,2.76,2.838,2.901,3.08,3.151,3.17,3.156,3.075,2.963,2.843,2.723,2.61,2.502,2.402,1.995,1.702,1.484,1.316,1.076,.9136,.7962,.7076,.6382,.5824,.5365};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_4[i]*dens[j]*factor);}
G4double Al_5[31]={2.047,2.247,2.586,2.86,3.084,3.269,3.421,3.545,3.646,3.96,4.113,4.188,4.214,4.181,4.086,3.966,3.838,3.709,3.584,3.464,2.952,2.562,2.261,2.022,1.67,1.425,1.245,1.107,.9991,.9116,.8395};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_5[i]*dens[j]*factor);}
G4double Al_6[31]={2.284,2.525,2.94,3.284,3.571,3.813,4.016,4.188,4.332,4.8,5.049,5.192,5.269,5.301,5.241,5.136,5.01,4.876,4.741,4.608,4.013,3.537,3.157,2.847,2.378,2.042,1.791,1.596,1.442,1.316,1.212};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_6[i]*dens[j]*factor);}
G4double Al_7[31]={2.478,2.753,3.234,3.641,3.988,4.286,4.541,4.761,4.95,5.583,5.941,6.164,6.302,6.419,6.409,6.331,6.218,6.088,5.951,5.812,5.157,4.609,4.156,3.779,3.192,2.76,2.431,2.173,1.966,1.797,1.655};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_7[i]*dens[j]*factor);}
G4double Al_8[31]={2.648,2.953,3.495,3.962,4.366,4.718,5.024,5.292,5.525,6.328,6.801,7.108,7.314,7.528,7.581,7.541,7.45,7.331,7.199,7.059,6.366,5.757,5.24,4.801,4.1,3.572,3.162,2.836,2.572,2.353,2.17};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_8[i]*dens[j]*factor);}
G4double Al_9[31]={2.795,3.126,3.719,4.238,4.693,5.094,5.448,5.762,6.039,7.017,7.606,8.001,8.275,8.59,8.709,8.712,8.648,8.544,8.418,8.281,7.563,6.906,6.336,5.843,5.044,4.428,3.942,3.551,3.23,2.963,2.737};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_9[i]*dens[j]*factor);}
G4double Al_10[31]={2.924,3.276,3.913,4.477,4.978,5.424,5.822,6.178,6.496,7.647,8.355,8.841,9.188,9.614,9.809,9.863,9.833,9.751,9.638,9.508,8.781,8.087,7.471,6.931,6.04,5.34,4.781,4.325,3.947,3.63,3.361};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_10[i]*dens[j]*factor);}
G4double Al_11[31]={3.037,3.406,4.083,4.69,5.235,5.725,6.167,6.566,6.926,8.264,9.11,9.701,1.14,1.7,1.99,11.12,11.14,11.09,11.01,1.89,1.19,9.479,8.825,8.24,7.25,6.456,5.808,5.273,4.826,4.446,4.121};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_11[i]*dens[j]*factor);}
G4double Al_12[31]={3.156,3.539,4.245,4.883,5.462,5.987,6.463,6.895,7.288,8.778,9.738,1.41,1.92,11.59,11.96,12.14,12.2,12.17,12.1,11.99,11.28,1.54,9.851,9.231,8.178,7.325,6.625,6.042,5.551,5.132,4.772};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_12[i]*dens[j]*factor);}
G4double Al_13[31]={3.267,3.662,4.395,5.063,5.672,6.23,6.739,7.205,7.631,9.28,1.36,11.14,11.72,12.52,12.99,13.24,13.35,13.36,13.31,13.23,12.55,11.79,11.07,1.42,9.298,8.375,7.609,6.967,6.421,5.953,5.547};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_13[i]*dens[j]*factor);}
G4double Al_14[31]={3.374,3.779,4.534,5.227,5.864,6.449,6.988,7.484,7.941,9.742,1.95,11.82,12.48,13.41,13.98,14.3,14.47,14.52,14.5,14.43,13.79,13.02,12.29,11.61,1.42,9.435,8.61,7.912,7.315,6.799,6.349};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_14[i]*dens[j]*factor);}
G4double Al_15[31]={3.473,3.887,4.662,5.378,6.039,6.65,7.216,7.74,8.226,1.17,11.51,12.47,13.22,14.28,14.96,15.38,15.61,15.72,15.74,15.7,15.12,14.36,13.61,12.91,11.66,1.62,9.727,8.969,8.314,7.745,7.248};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_15[i]*dens[j]*factor);}
G4double Al_16[31]={3.569,3.992,4.787,5.523,6.208,6.844,7.436,7.987,8.5,1.59,12.05,13.12,13.94,15.15,15.94,16.44,16.74,16.9,16.97,16.96,16.46,15.72,14.96,14.24,12.95,11.84,1.89,1.07,9.364,8.744,8.198};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_16[i]*dens[j]*factor);}
G4double Al_17[31]={3.673,4.103,4.913,5.666,6.369,7.025,7.639,8.213,8.751,1.97,12.55,13.72,14.63,15.97,16.87,17.46,17.84,18.05,18.16,18.19,17.77,17.05,16.29,15.55,14.21,13.05,12.05,11.18,1.42,9.75,9.159};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_17[i]*dens[j]*factor);}
G4double Al_18[31]={3.756,4.193,5.016,5.782,6.499,7.171,7.803,8.397,8.956,11.29,12.99,14.25,15.24,16.7,17.7,18.38,18.82,19.09,19.24,19.3,18.96,18.26,17.49,16.74,15.37,14.16,13.12,12.2,11.4,1.69,1.07};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Al_18[i]*dens[j]*factor);}
j++;

G4double Si_3[31]={1.468,1.612,1.817,1.946,2.033,2.093,2.137,2.169,2.192,2.242,2.237,2.208,2.167,2.066,1.96,1.856,1.759,1.67,1.589,1.514,1.224,1.027,.8863,.7809,.634,.5366,.4672,.4151,.3745,.3418,.3149};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_3[i]*dens[j]*factor);}
G4double Si_4[31]={1.792,1.998,2.323,2.55,2.71,2.828,2.918,2.988,3.043,3.195,3.247,3.254,3.236,3.155,3.047,2.929,2.811,2.697,2.589,2.488,2.071,1.77,1.545,1.371,1.122,.9528,.8305,.7381,.6658,.6075,.5596};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_4[i]*dens[j]*factor);}
G4double Si_5[31]={2.047,2.299,2.729,3.055,3.297,3.48,3.623,3.738,3.831,4.113,4.241,4.3,4.318,4.283,4.193,4.079,3.954,3.827,3.702,3.581,3.06,2.66,2.35,2.103,1.739,1.484,1.297,1.154,1.042,.9505,.8753};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_5[i]*dens[j]*factor);}
G4double Si_6[31]={2.263,2.546,3.059,3.478,3.804,4.057,4.259,4.424,4.56,4.992,5.211,5.333,5.397,5.424,5.37,5.272,5.152,5.022,4.889,4.757,4.154,3.667,3.276,2.957,2.473,2.125,1.864,1.662,1.502,1.371,1.263};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_6[i]*dens[j]*factor);}
G4double Si_7[31]={2.453,2.758,3.329,3.825,4.23,4.555,4.819,5.036,5.218,5.814,6.138,6.333,6.454,6.561,6.557,6.489,6.386,6.262,6.129,5.992,5.333,4.773,4.308,3.92,3.315,2.869,2.529,2.261,2.047,1.871,1.724};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_7[i]*dens[j]*factor);}
G4double Si_8[31]={2.634,2.956,3.568,4.125,4.603,4.997,5.323,5.595,5.825,6.596,7.031,7.307,7.49,7.688,7.746,7.719,7.64,7.531,7.404,7.268,6.575,5.955,5.425,4.974,4.254,3.709,3.285,2.949,2.675,2.448,2.259};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_8[i]*dens[j]*factor);}
G4double Si_9[31]={2.803,3.14,3.782,4.384,4.921,5.379,5.765,6.091,6.369,7.318,7.869,8.227,8.475,8.767,8.889,8.907,8.858,8.767,8.651,8.521,7.809,7.143,6.56,6.055,5.232,4.598,4.096,3.691,3.359,3.083,2.849};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_9[i]*dens[j]*factor);}
G4double Si_10[31]={2.965,3.318,3.984,4.616,5.2,5.715,6.158,6.537,6.864,7.996,8.666,9.112,9.426,9.819,1.01,1.08,1.07,10,9.901,9.779,9.063,8.361,7.732,7.179,6.263,5.542,4.965,4.494,4.103,3.775,3.496};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_10[i]*dens[j]*factor);}
G4double Si_11[31]={3.107,3.473,4.161,4.817,5.437,6.001,6.496,6.927,7.302,8.623,9.423,9.963,1.35,1.86,11.13,11.26,11.29,11.25,11.17,11.07,1.36,9.634,8.966,8.369,7.365,6.562,5.909,5.37,4.919,4.537,4.21};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_11[i]*dens[j]*factor);}
G4double Si_12[31]={3.257,3.639,4.347,5.018,5.665,6.268,6.809,7.287,7.707,9.209,1.13,1.77,11.23,11.85,12.21,12.4,12.47,12.47,12.41,12.32,11.64,1.89,1.19,9.554,8.473,7.595,6.874,6.273,5.765,5.333,4.96};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_12[i]*dens[j]*factor);}
G4double Si_13[31]={3.396,3.794,4.523,5.207,5.87,6.502,7.083,7.604,8.067,9.75,1.81,11.54,12.08,12.81,13.26,13.52,13.65,13.68,13.65,13.58,12.94,12.18,11.45,1.78,9.63,8.682,7.894,7.231,6.668,6.184,5.764};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_13[i]*dens[j]*factor);}
G4double Si_14[31]={3.53,3.941,4.694,5.39,6.063,6.716,7.328,7.887,8.39,1.25,11.43,12.26,12.88,13.73,14.27,14.6,14.79,14.86,14.87,14.82,14.22,13.45,12.7,12.01,1.79,9.782,8.932,8.212,7.595,7.062,6.598};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_14[i]*dens[j]*factor);}
G4double Si_15[31]={3.652,4.078,4.853,5.562,6.244,6.91,7.547,8.139,8.678,1.71,12.03,12.96,13.66,14.64,15.29,15.7,15.95,16.08,16.12,16.1,15.58,14.83,14.07,13.35,12.08,11,1.08,9.303,8.628,8.042,7.527};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_15[i]*dens[j]*factor);}
G4double Si_16[31]={3.768,4.21,5.011,5.738,6.429,7.107,7.763,8.382,8.954,11.15,12.6,13.64,14.42,15.54,16.29,16.79,17.1,17.29,17.37,17.39,16.96,16.22,15.46,14.72,13.4,12.26,11.28,1.44,9.713,9.073,8.509};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_16[i]*dens[j]*factor);}
G4double Si_17[31]={3.887,4.344,5.168,5.91,6.609,7.292,7.961,8.602,9.201,11.55,13.14,14.28,15.14,16.39,17.24,17.83,18.21,18.45,18.58,18.64,18.3,17.59,16.82,16.07,14.7,13.51,12.48,11.59,1.8,1.12,9.505};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_17[i]*dens[j]*factor);}
G4double Si_18[31]={3.994,4.465,5.314,6.074,6.781,7.469,8.146,8.803,9.426,11.92,13.63,14.87,15.82,17.2,18.15,18.82,19.27,19.56,19.74,19.83,19.57,18.89,18.12,17.35,15.94,14.7,13.62,12.68,11.85,11.12,1.47};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Si_18[i]*dens[j]*factor);}
j++;

G4double Ar_3[31]={1.288,1.41,1.603,1.742,1.838,1.904,1.949,1.979,1.998,2.017,1.985,1.936,1.882,1.773,1.672,1.58,1.498,1.423,1.355,1.293,1.053,.8894,.7709,.6814,.5556,.4714,.4111,.3656,.3301,.3014,.2778};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_3[i]*dens[j]*factor);}
G4double Ar_4[31]={1.578,1.748,2.032,2.253,2.422,2.549,2.645,2.716,2.769,2.886,2.898,2.87,2.825,2.716,2.604,2.495,2.393,2.297,2.207,2.123,1.779,1.528,1.34,1.193,.9809,.8355,.7297,.6495,.5864,.5355,.4936};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_4[i]*dens[j]*factor);}
G4double Ar_5[31]={1.804,2.017,2.382,2.68,2.92,3.111,3.261,3.38,3.474,3.723,3.801,3.81,3.787,3.699,3.591,3.479,3.367,3.258,3.154,3.054,2.624,2.292,2.032,1.825,1.516,1.299,1.138,1.014,.9162,.837,.7714};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_5[i]*dens[j]*factor);}
G4double Ar_6[31]={1.988,2.236,2.672,3.04,3.347,3.601,3.808,3.978,4.116,4.52,4.682,4.742,4.752,4.7,4.609,4.502,4.39,4.276,4.164,4.054,3.559,3.155,2.828,2.561,2.151,1.855,1.632,1.458,1.319,1.206,1.112};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_6[i]*dens[j]*factor);}
G4double Ar_7[31]={2.142,2.417,2.912,3.34,3.708,4.02,4.283,4.504,4.689,5.262,5.524,5.648,5.701,5.701,5.638,5.547,5.442,5.331,5.218,5.104,4.564,4.101,3.713,3.388,2.878,2.5,2.209,1.98,1.795,1.643,1.516};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_7[i]*dens[j]*factor);}
G4double Ar_8[31]={2.285,2.582,3.126,3.606,4.027,4.394,4.71,4.98,5.212,5.963,6.336,6.532,6.635,6.7,6.673,6.605,6.515,6.412,6.302,6.188,5.621,5.111,4.67,4.292,3.686,3.225,2.865,2.576,2.342,2.147,1.983};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_8[i]*dens[j]*factor);}
G4double Ar_9[31]={2.419,2.731,3.314,3.838,4.306,4.721,5.085,5.402,5.678,6.609,7.1,7.374,7.533,7.667,7.682,7.64,7.567,7.475,7.372,7.261,6.682,6.134,5.649,5.226,4.534,3.996,3.57,3.224,2.94,2.702,2.501};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_9[i]*dens[j]*factor);}
G4double Ar_10[31]={2.55,2.874,3.487,4.048,4.556,5.014,5.422,5.783,6.101,7.21,7.824,8.183,8.402,8.616,8.679,8.668,8.616,8.539,8.446,8.342,7.76,7.185,6.662,6.199,5.428,4.818,4.327,3.925,3.59,3.308,3.068};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_10[i]*dens[j]*factor);}
G4double Ar_11[31]={2.671,3.004,3.641,4.233,4.776,5.271,5.718,6.12,6.478,7.764,8.509,8.961,9.249,9.555,9.674,9.701,9.676,9.617,9.537,9.443,8.874,8.28,7.726,7.227,6.381,5.7,5.145,4.686,4.3,3.973,3.692};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_11[i]*dens[j]*factor);}
G4double Ar_12[31]={2.799,3.14,3.794,4.41,4.981,5.509,5.992,6.43,6.826,8.279,9.153,9.699,1.06,1.46,1.64,1.71,1.71,1.67,1.61,1.52,9.972,9.364,8.785,8.254,7.342,6.599,5.986,5.473,5.039,4.668,4.348};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_12[i]*dens[j]*factor);}
G4double Ar_13[31]={2.923,3.272,3.942,4.577,5.173,5.727,6.24,6.712,7.142,8.757,9.761,1.41,1.84,11.34,11.59,11.7,11.74,11.73,11.68,11.61,11.09,1.48,9.876,9.319,8.347,7.543,6.873,6.308,5.826,5.411,5.051};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_13[i]*dens[j]*factor);}
G4double Ar_14[31]={3.044,3.402,4.085,4.736,5.351,5.928,6.467,6.967,7.428,9.196,1.33,11.07,11.58,12.19,12.51,12.67,12.74,12.76,12.73,12.68,12.2,11.58,1.97,1.39,9.361,8.502,7.779,7.165,6.637,6.181,5.782};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_14[i]*dens[j]*factor);}
G4double Ar_15[31]={3.158,3.525,4.22,4.883,5.514,6.11,6.671,7.197,7.686,9.599,1.86,11.71,12.3,13.02,13.42,13.65,13.76,13.81,13.81,13.78,13.36,12.76,12.14,11.54,1.46,9.552,8.774,8.108,7.532,7.03,6.589};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_15[i]*dens[j]*factor);}
G4double Ar_16[31]={3.27,3.647,4.357,5.032,5.678,6.291,6.872,7.42,7.933,9.984,11.37,12.32,13,13.85,14.33,14.61,14.78,14.86,14.9,14.89,14.53,13.95,13.33,12.72,11.6,1.64,9.809,9.093,8.47,7.923,7.441};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_16[i]*dens[j]*factor);}
G4double Ar_17[31]={3.382,3.77,4.494,5.178,5.835,6.462,7.059,7.625,8.16,1.34,11.84,12.9,13.66,14.63,15.2,15.55,15.76,15.88,15.95,15.96,15.68,15.13,14.51,13.89,12.73,11.72,1.85,1.09,9.419,8.83,8.309};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_17[i]*dens[j]*factor);}
G4double Ar_18[31]={3.489,3.89,4.63,5.325,5.991,6.63,7.24,7.822,8.376,1.67,12.29,13.45,14.29,15.39,16.04,16.45,16.71,16.87,16.97,17.01,16.8,16.26,15.64,15.01,13.82,12.77,11.85,11.04,1.34,9.712,9.155};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ar_18[i]*dens[j]*factor);}
j++;

G4double Ti_3[31]={1.008,1.108,1.272,1.4,1.5,1.577,1.635,1.68,1.715,1.785,1.777,1.743,1.7,1.612,1.527,1.448,1.376,1.311,1.251,1.197,.9831,.8357,.7282,.6465,.5304,.452,.3954,.3525,.3188,.2916,.269};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_3[i]*dens[j]*factor);}
G4double Ti_4[31]={1.218,1.356,1.594,1.789,1.949,2.081,2.187,2.274,2.345,2.538,2.587,2.579,2.548,2.464,2.372,2.281,2.193,2.11,2.032,1.959,1.656,1.433,1.263,1.13,.9349,.7999,.701,.6255,.5659,.5177,.4777};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_4[i]*dens[j]*factor);}
G4double Ti_5[31]={1.374,1.544,1.846,2.103,2.321,2.507,2.664,2.796,2.907,3.249,3.381,3.419,3.414,3.353,3.268,3.175,3.081,2.988,2.898,2.812,2.438,2.145,1.912,1.725,1.442,1.241,1.091,.9755,.8833,.8084,.7461};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_5[i]*dens[j]*factor);}
G4double Ti_6[31]={1.498,1.692,2.047,2.358,2.631,2.869,3.075,3.253,3.406,3.912,4.147,4.247,4.279,4.258,4.19,4.105,4.012,3.916,3.821,3.727,3.3,2.947,2.657,2.416,2.043,1.77,1.563,1.401,1.27,1.164,1.075};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_6[i]*dens[j]*factor);}
G4double Ti_7[31]={1.6,1.812,2.207,2.563,2.883,3.168,3.42,3.643,3.839,4.516,4.869,5.044,5.125,5.16,5.122,5.053,4.968,4.876,4.781,4.685,4.224,3.823,3.481,3.191,2.73,2.382,2.113,1.9,1.727,1.583,1.464};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_7[i]*dens[j]*factor);}
G4double Ti_8[31]={1.696,1.922,2.348,2.742,3.102,3.428,3.723,3.988,4.225,5.076,5.555,5.816,5.956,6.06,6.06,6.014,5.944,5.86,5.768,5.673,5.194,4.756,4.371,4.036,3.49,3.068,2.736,2.468,2.249,2.067,1.912};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_8[i]*dens[j]*factor);}
G4double Ti_9[31]={1.789,2.024,2.474,2.896,3.289,3.651,3.984,4.287,4.561,5.582,6.192,6.547,6.751,6.932,6.976,6.956,6.903,6.831,6.746,6.655,6.17,5.705,5.284,4.911,4.29,3.8,3.407,3.087,2.822,2.599,2.41};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_9[i]*dens[j]*factor);}
G4double Ti_10[31]={1.883,2.125,2.593,3.037,3.457,3.849,4.214,4.551,4.86,6.044,6.786,7.241,7.517,7.788,7.882,7.894,7.862,7.804,7.73,7.645,7.164,6.679,6.23,5.824,5.134,4.58,4.129,3.756,3.445,3.181,2.955};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_10[i]*dens[j]*factor);}
G4double Ti_11[31]={1.969,2.218,2.7,3.162,3.603,4.02,4.413,4.78,5.121,6.462,7.338,7.9,8.255,8.629,8.784,8.835,8.83,8.79,8.728,8.653,8.186,7.69,7.218,6.783,6.031,5.415,4.906,4.482,4.124,3.818,3.554};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_11[i]*dens[j]*factor);}
G4double Ti_12[31]={2.062,2.316,2.808,3.283,3.74,4.178,4.594,4.987,5.356,6.841,7.846,8.514,8.953,9.436,9.658,9.753,9.777,9.758,9.711,9.646,9.197,8.694,8.204,7.745,6.939,6.267,5.706,5.233,4.831,4.484,4.184};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_12[i]*dens[j]*factor);}
G4double Ti_13[31]={2.154,2.414,2.917,3.402,3.873,4.326,4.762,5.177,5.57,7.192,8.324,9.101,9.626,1.23,1.52,1.66,1.72,1.72,1.7,1.64,1.23,9.723,9.219,8.741,7.886,7.162,6.55,6.029,5.583,5.196,4.859};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_13[i]*dens[j]*factor);}
G4double Ti_14[31]={2.246,2.512,3.023,3.518,3.998,4.465,4.915,5.348,5.762,7.509,8.763,9.648,1.26,1.98,11.35,11.55,11.64,11.67,11.66,11.63,11.25,1.75,1.24,9.74,8.842,8.071,7.413,6.848,6.359,5.934,5.561};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_14[i]*dens[j]*factor);}
G4double Ti_15[31]={2.33,2.602,3.122,3.624,4.112,4.589,5.052,5.5,5.932,7.792,9.166,1.16,1.86,11.72,12.18,12.43,12.57,12.64,12.65,12.64,12.31,11.83,11.32,1.81,9.876,9.059,8.354,7.742,7.21,6.744,6.333};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_15[i]*dens[j]*factor);}
G4double Ti_16[31]={2.246,2.512,3.023,3.518,3.998,4.465,4.915,5.348,5.762,7.509,8.763,9.648,1.26,1.98,11.35,11.55,11.64,11.67,11.66,11.63,11.25,1.75,1.24,9.74,8.842,8.071,7.413,6.848,6.359,5.934,5.561};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_16[i]*dens[j]*factor);}
G4double Ti_17[31]={2.5,2.786,3.327,3.844,4.346,4.838,5.32,5.792,6.25,8.302,9.896,11.1,11.99,13.13,13.77,14.16,14.39,14.53,14.61,14.64,14.45,14.01,13.51,12.99,12,11.1,1.32,9.62,9.006,8.461,7.976};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_17[i]*dens[j]*factor);}
G4double Ti_18[31]={2.585,2.879,3.433,3.958,4.467,4.965,5.454,5.934,6.403,8.533,1.23,11.53,12.52,13.8,14.53,14.99,15.27,15.45,15.55,15.6,15.48,15.07,14.57,14.04,13.02,12.1,11.27,1.54,9.887,9.308,8.791};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ti_18[i]*dens[j]*factor);}
j++;

G4double Fe_3[31]={.8835,.9749,1.128,1.249,1.346,1.422,1.481,1.526,1.561,1.644,1.652,1.63,1.595,1.515,1.436,1.363,1.297,1.236,1.18,1.13,.9314,.7945,.6945,.6183,.5096,.4358,.3822,.3414,.3092,.2831,.2615};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_3[i]*dens[j]*factor);}
G4double Fe_4[31]={1.055,1.181,1.398,1.58,1.732,1.859,1.964,2.051,2.121,2.322,2.391,2.401,2.383,2.312,2.227,2.142,2.061,1.984,1.912,1.844,1.565,1.359,1.202,1.078,.8965,.7699,.6767,.6051,.5484,.5024,.4642};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_4[i]*dens[j]*factor);}
G4double Fe_5[31]={1.178,1.33,1.603,1.839,2.044,2.221,2.373,2.502,2.612,2.956,3.108,3.17,3.182,3.14,3.064,2.978,2.891,2.805,2.722,2.642,2.298,2.029,1.815,1.642,1.38,1.193,1.052,.9424,.8551,.7837,.7243};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_5[i]*dens[j]*factor);}
G4double Fe_6[31]={1.273,1.446,1.764,2.047,2.299,2.522,2.719,2.892,3.042,3.543,3.794,3.92,3.976,3.98,3.924,3.846,3.76,3.672,3.584,3.497,3.105,2.782,2.516,2.295,1.952,1.698,1.504,1.351,1.228,1.127,1.042};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_6[i]*dens[j]*factor);}
G4double Fe_7[31]={1.349,1.535,1.886,2.208,2.5,2.765,3.003,3.217,3.407,4.072,4.434,4.636,4.745,4.815,4.792,4.731,4.654,4.569,4.481,4.392,3.968,3.602,3.291,3.026,2.602,2.281,2.03,1.83,1.667,1.532,1.418};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_7[i]*dens[j]*factor);}
G4double Fe_8[31]={1.422,1.617,1.994,2.347,2.674,2.974,3.249,3.5,3.727,4.558,5.037,5.324,5.495,5.643,5.663,5.627,5.564,5.487,5.403,5.315,4.875,4.475,4.126,3.821,3.321,2.933,2.624,2.374,2.168,1.996,1.85};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_8[i]*dens[j]*factor);}
G4double Fe_9[31]={1.492,1.695,2.089,2.466,2.82,3.151,3.458,3.741,4.002,4.993,5.594,5.97,6.208,6.444,6.514,6.508,6.463,6.397,6.319,6.235,5.788,5.364,4.984,4.646,4.079,3.629,3.265,2.967,2.718,2.509,2.33};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_9[i]*dens[j]*factor);}
G4double Fe_10[31]={1.564,1.772,2.179,2.574,2.951,3.306,3.64,3.953,4.244,5.386,6.111,6.58,6.89,7.226,7.354,7.383,7.36,7.309,7.241,7.163,6.721,6.279,5.872,5.505,4.879,4.37,3.953,3.607,3.316,3.068,2.855};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_10[i]*dens[j]*factor);}
G4double Fe_11[31]={1.632,1.845,2.262,2.67,3.064,3.441,3.799,4.136,4.454,5.738,6.588,7.155,7.543,7.99,8.187,8.259,8.264,8.232,8.177,8.108,7.678,7.226,6.799,6.407,5.726,5.162,4.694,4.3,3.965,3.679,3.431};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_11[i]*dens[j]*factor);}
G4double Fe_12[31]={1.706,1.923,2.348,2.765,3.173,3.566,3.943,4.302,4.643,6.055,7.026,7.69,8.156,8.719,8.992,9.113,9.15,9.14,9.1,9.041,8.628,8.169,7.726,7.314,6.585,5.972,5.456,5.018,4.643,4.319,4.037};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_12[i]*dens[j]*factor);}
G4double Fe_13[31]={1.778,2,2.432,2.857,3.274,3.681,4.074,4.451,4.811,6.343,7.433,8.196,8.742,9.426,9.779,9.955,1.03,1.05,1.02,9.979,9.594,9.135,8.681,8.25,7.479,6.82,6.259,5.778,5.363,5.002,4.685};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_13[i]*dens[j]*factor);}
G4double Fe_14[31]={1.851,2.078,2.517,2.948,3.373,3.79,4.196,4.588,4.965,6.603,7.808,8.669,9.296,1.1,1.54,1.77,1.89,1.93,1.93,1.9,1.55,1.1,9.637,9.193,8.384,7.684,7.082,6.56,6.107,5.71,5.36};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_14[i]*dens[j]*factor);}
G4double Fe_15[31]={1.919,2.151,2.597,3.033,3.463,3.888,4.305,4.709,5.101,6.833,8.149,9.109,9.819,1.76,11.29,11.59,11.75,11.83,11.86,11.85,11.56,11.12,1.65,1.2,9.358,8.618,7.974,7.411,6.918,6.484,6.099};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_15[i]*dens[j]*factor);}
G4double Fe_16[31]={1.987,2.225,2.681,3.122,3.559,3.991,4.416,4.831,5.235,7.05,8.47,9.527,1.32,11.4,12.03,12.4,12.61,12.73,12.78,12.8,12.56,12.14,11.68,11.22,1.36,9.581,8.898,8.296,7.765,7.294,6.875};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_16[i]*dens[j]*factor);}
G4double Fe_17[31]={2.055,2.3,2.764,3.211,3.651,4.087,4.519,4.943,5.357,7.244,8.76,9.911,1.79,12,12.73,13.18,13.44,13.6,13.68,13.72,13.56,13.16,12.71,12.24,11.36,1.55,9.832,9.195,8.628,8.123,7.671};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_17[i]*dens[j]*factor);}
G4double Fe_18[31]={2.127,2.378,2.854,3.308,3.753,4.194,4.631,5.062,5.485,7.434,9.04,1.28,11.24,12.58,13.42,13.94,14.26,14.46,14.57,14.63,14.53,14.16,13.71,13.24,12.33,11.49,1.74,1.07,9.473,8.937,8.455};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Fe_18[i]*dens[j]*factor);}
j++;

G4double Ni_3[31]={.8314,.9198,1.07,1.193,1.294,1.376,1.443,1.495,1.536,1.634,1.649,1.632,1.601,1.525,1.447,1.375,1.308,1.247,1.192,1.141,.942,.8042,.7035,.6266,.5169,.4423,.3881,.3468,.3142,.2878,.266};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_3[i]*dens[j]*factor);}
G4double Ni_4[31]={.9896,1.109,1.32,1.5,1.654,1.786,1.899,1.994,2.072,2.301,2.382,2.399,2.387,2.322,2.24,2.157,2.076,1.999,1.927,1.859,1.579,1.373,1.215,1.091,.9083,.7808,.6867,.6144,.5571,.5105,.4718};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_4[i]*dens[j]*factor);}
G4double Ni_5[31]={1.103,1.245,1.506,1.737,1.941,2.12,2.278,2.417,2.537,2.92,3.091,3.162,3.183,3.151,3.08,2.995,2.908,2.823,2.74,2.66,2.316,2.046,1.832,1.659,1.397,1.208,1.066,.9562,.8681,.796,.736};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_5[i]*dens[j]*factor);}
G4double Ni_6[31]={1.191,1.35,1.649,1.923,2.17,2.393,2.594,2.774,2.934,3.485,3.763,3.904,3.971,3.991,3.942,3.867,3.781,3.693,3.605,3.518,3.125,2.802,2.537,2.316,1.972,1.718,1.523,1.37,1.246,1.144,1.058};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_6[i]*dens[j]*factor);}
G4double Ni_7[31]={1.262,1.432,1.759,2.065,2.35,2.611,2.85,3.068,3.266,3.989,4.387,4.609,4.732,4.822,4.809,4.753,4.677,4.592,4.504,4.415,3.991,3.625,3.315,3.05,2.627,2.305,2.054,1.853,1.69,1.553,1.439};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_7[i]*dens[j]*factor);}
G4double Ni_8[31]={1.329,1.508,1.855,2.187,2.502,2.796,3.069,3.322,3.555,4.445,4.972,5.283,5.471,5.645,5.679,5.65,5.59,5.513,5.429,5.341,4.899,4.5,4.151,3.847,3.349,2.961,2.653,2.402,2.196,2.023,1.876};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_8[i]*dens[j]*factor);}
G4double Ni_9[31]={1.395,1.581,1.942,2.293,2.631,2.952,3.254,3.536,3.8,4.848,5.506,5.914,6.173,6.44,6.529,6.533,6.492,6.427,6.35,6.265,5.816,5.392,5.012,4.675,4.111,3.662,3.299,3,2.751,2.541,2.361};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_9[i]*dens[j]*factor);}
G4double Ni_10[31]={1.461,1.653,2.026,2.39,2.746,3.088,3.414,3.722,4.013,5.207,5.998,6.507,6.842,7.213,7.366,7.409,7.392,7.343,7.276,7.198,6.751,6.309,5.903,5.538,4.914,4.408,3.992,3.646,3.355,3.107,2.893};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_10[i]*dens[j]*factor);}
G4double Ni_11[31]={1.522,1.719,2.102,2.477,2.846,3.205,3.552,3.883,4.198,5.524,6.447,7.062,7.479,7.967,8.194,8.284,8.298,8.269,8.215,8.146,7.713,7.259,6.833,6.442,5.765,5.204,4.737,4.344,4.01,3.723,3.474};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_11[i]*dens[j]*factor);}
G4double Ni_12[31]={1.588,1.791,2.183,2.566,2.944,3.316,3.679,4.029,4.364,5.809,6.856,7.576,8.076,8.685,8.993,9.136,9.186,9.18,9.142,9.084,8.668,8.207,7.764,7.353,6.628,6.019,5.505,5.068,4.693,4.369,4.087};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_12[i]*dens[j]*factor);}
G4double Ni_13[31]={1.652,1.86,2.261,2.651,3.037,3.418,3.793,4.158,4.511,6.061,7.229,8.057,8.643,9.378,9.772,9.974,1.06,1.09,1.07,1.03,9.64,9.177,8.722,8.293,7.525,6.871,6.313,5.833,5.419,5.058,4.741};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_13[i]*dens[j]*factor);}
G4double Ni_14[31]={1.717,1.93,2.34,2.737,3.129,3.517,3.901,4.278,4.645,6.29,7.571,8.504,9.178,1.04,1.53,1.79,1.92,1.98,1.98,1.96,1.61,1.15,9.683,9.239,8.435,7.74,7.141,6.621,6.169,5.772,5.423};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_14[i]*dens[j]*factor);}
G4double Ni_15[31]={1.776,1.994,2.411,2.814,3.211,3.605,3.996,4.382,4.761,6.489,7.879,8.917,9.681,1.68,11.27,11.6,11.79,11.88,11.91,11.91,11.61,11.17,1.7,1.25,9.41,8.676,8.036,7.476,6.985,6.551,6.166};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_15[i]*dens[j]*factor);}
G4double Ni_16[31]={1.837,2.061,2.488,2.899,3.302,3.701,4.098,4.492,4.881,6.679,8.166,9.307,1.16,11.3,11.99,12.4,12.64,12.77,12.84,12.86,12.62,12.2,11.73,11.27,1.41,9.641,8.963,8.365,7.836,7.366,6.947};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_16[i]*dens[j]*factor);}
G4double Ni_17[31]={1.899,2.127,2.563,2.981,3.389,3.792,4.193,4.593,4.989,6.847,8.422,9.66,1.6,11.89,12.68,13.17,13.47,13.64,13.74,13.78,13.62,13.22,12.76,12.3,11.41,1.61,9.901,9.267,8.703,8.2,7.749};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_17[i]*dens[j]*factor);}
G4double Ni_18[31]={1.965,2.199,2.645,3.072,3.486,3.894,4.299,4.704,5.106,7.015,8.67,10,11.03,12.46,13.35,13.92,14.28,14.5,14.63,14.7,14.6,14.22,13.77,13.3,12.39,11.56,1.82,1.15,9.555,9.021,8.54};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ni_18[i]*dens[j]*factor);}
j++;

G4double Cu_3[31]={.7526,.8347,.9738,1.086,1.178,1.253,1.313,1.361,1.4,1.494,1.511,1.499,1.473,1.406,1.336,1.271,1.21,1.155,1.104,1.058,.8753,.7485,.6555,.5845,.483,.4138,.3635,.3251,.2947,.2701,.2497};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_3[i]*dens[j]*factor);}
G4double Cu_4[31]={.8906,1.001,1.197,1.363,1.504,1.624,1.727,1.814,1.887,2.102,2.181,2.201,2.193,2.14,2.068,1.993,1.919,1.85,1.784,1.723,1.467,1.277,1.132,1.017,.8484,.7303,.643,.5758,.5225,.479,.443};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_4[i]*dens[j]*factor);}
G4double Cu_5[31]={.9874,1.119,1.361,1.574,1.762,1.926,2.07,2.197,2.307,2.666,2.829,2.9,2.924,2.902,2.841,2.767,2.689,2.612,2.537,2.464,2.15,1.903,1.706,1.547,1.304,1.13,.9983,.8959,.8139,.7468,.6908};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_5[i]*dens[j]*factor);}
G4double Cu_6[31]={1.062,1.209,1.486,1.739,1.966,2.171,2.355,2.52,2.667,3.18,3.443,3.578,3.645,3.673,3.636,3.571,3.496,3.417,3.337,3.258,2.901,2.605,2.361,2.158,1.841,1.606,1.426,1.283,1.168,1.073,.9932};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_6[i]*dens[j]*factor);}
G4double Cu_7[31]={1.121,1.277,1.579,1.862,2.124,2.364,2.584,2.784,2.965,3.636,4.011,4.222,4.341,4.436,4.434,4.389,4.323,4.248,4.169,4.089,3.704,3.369,3.085,2.842,2.452,2.154,1.922,1.736,1.583,1.457,1.35};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_7[i]*dens[j]*factor);}
G4double Cu_8[31]={1.18,1.343,1.662,1.968,2.258,2.528,2.779,3.011,3.225,4.049,4.543,4.838,5.018,5.191,5.235,5.216,5.166,5.099,5.024,4.946,4.546,4.181,3.862,3.583,3.125,2.767,2.481,2.249,2.057,1.897,1.76};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_8[i]*dens[j]*factor);}
G4double Cu_9[31]={1.238,1.406,1.736,2.058,2.369,2.664,2.942,3.202,3.444,4.412,5.03,5.414,5.659,5.92,6.017,6.03,5.999,5.944,5.876,5.801,5.396,5.01,4.662,4.353,3.835,3.421,3.085,2.808,2.577,2.382,2.215};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_9[i]*dens[j]*factor);}
G4double Cu_10[31]={1.298,1.47,1.808,2.141,2.468,2.783,3.083,3.366,3.634,4.735,5.475,5.954,6.27,6.629,6.786,6.837,6.83,6.791,6.733,6.665,6.264,5.862,5.491,5.156,4.584,4.117,3.733,3.413,3.142,2.912,2.713};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_10[i]*dens[j]*factor);}
G4double Cu_11[31]={1.354,1.53,1.875,2.217,2.555,2.885,3.203,3.508,3.797,5.02,5.882,6.459,6.852,7.319,7.546,7.642,7.666,7.647,7.602,7.543,7.156,6.744,6.355,5.998,5.376,4.86,4.429,4.065,3.755,3.489,3.257};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_11[i]*dens[j]*factor);}
G4double Cu_12[31]={1.416,1.596,1.947,2.294,2.639,2.98,3.314,3.636,3.944,5.275,6.252,6.927,7.397,7.976,8.279,8.427,8.485,8.489,8.46,8.411,8.042,7.625,7.221,6.845,6.181,5.62,5.146,4.742,4.395,4.094,3.831};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_12[i]*dens[j]*factor);}
G4double Cu_13[31]={1.476,1.66,2.017,2.368,2.719,3.068,3.412,3.747,4.072,5.5,6.588,7.363,7.913,8.609,8.992,9.197,9.295,9.327,9.319,9.284,8.944,8.526,8.112,7.72,7.017,6.415,5.9,5.457,5.073,4.738,4.444};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_13[i]*dens[j]*factor);}
G4double Cu_14[31]={1.536,1.725,2.089,2.445,2.799,3.154,3.506,3.852,4.19,5.705,6.897,7.77,8.401,9.217,9.684,9.948,1.09,1.15,1.16,1.14,9.841,9.427,9.005,8.601,7.865,7.226,6.674,6.194,5.775,5.408,5.083};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_14[i]*dens[j]*factor);}
G4double Cu_15[31]={1.592,1.785,2.156,2.516,2.873,3.231,3.589,3.944,4.292,5.883,7.173,8.144,8.859,9.801,1.36,1.69,1.88,1.98,11.02,11.02,1.77,1.37,9.95,9.537,8.772,8.099,7.509,6.993,6.538,6.136,5.779};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_15[i]*dens[j]*factor);}
G4double Cu_16[31]={1.649,1.848,2.227,2.592,2.953,3.314,3.677,4.038,4.395,6.052,7.43,8.496,9.295,1.37,11.02,11.43,11.67,11.81,11.88,11.9,11.71,11.33,1.91,1.49,9.704,8.999,8.375,7.823,7.334,6.899,6.511};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_16[i]*dens[j]*factor);}
G4double Cu_17[31]={1.706,1.909,2.296,2.667,3.031,3.394,3.759,4.125,4.489,6.2,7.659,8.816,9.697,1.9,11.65,12.13,12.43,12.61,12.71,12.76,12.63,12.28,11.87,11.44,1.64,9.906,9.25,8.666,8.145,7.679,7.261};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_17[i]*dens[j]*factor);}
G4double Cu_18[31]={1.766,1.975,2.372,2.75,3.119,3.485,3.853,4.222,4.591,6.35,7.882,9.125,1.09,11.42,12.27,12.82,13.18,13.4,13.53,13.61,13.55,13.21,12.8,12.38,11.55,1.79,1.11,9.493,8.942,8.448,8.002};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Cu_18[i]*dens[j]*factor);}
j++;

G4double Ge_3[31]={.6441,.7195,.842,.9317,.9987,1.05,1.091,1.124,1.152,1.233,1.266,1.272,1.263,1.222,1.171,1.119,1.069,1.023,.9797,.9402,.7833,.673,.5915,.5289,.439,.3774,.3323,.2979,.2705,.2483,.2298};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_3[i]*dens[j]*factor);}
G4double Ge_4[31]={.7658,.8635,1.037,1.178,1.29,1.379,1.452,1.513,1.565,1.733,1.819,1.86,1.874,1.855,1.809,1.753,1.695,1.637,1.582,1.53,1.311,1.147,1.02,.9192,.7703,.6653,.5874,.5272,.4793,.4401,.4075};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_4[i]*dens[j]*factor);}
G4double Ge_5[31]={.8579,.9711,1.182,1.367,1.522,1.651,1.759,1.85,1.929,2.198,2.352,2.441,2.488,2.51,2.482,2.432,2.373,2.311,2.249,2.188,1.92,1.707,1.536,1.396,1.183,1.028,.9111,.8196,.746,.6857,.6352};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_5[i]*dens[j]*factor);}
G4double Ge_6[31]={.9323,1.057,1.294,1.512,1.706,1.873,2.016,2.139,2.247,2.626,2.856,3.001,3.091,3.168,3.17,3.136,3.083,3.022,2.958,2.893,2.59,2.335,2.123,1.946,1.668,1.46,1.3,1.173,1.07,.9843,.9125};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_6[i]*dens[j]*factor);}
G4double Ge_7[31]={.9928,1.126,1.382,1.623,1.847,2.047,2.224,2.379,2.515,3.01,3.321,3.53,3.669,3.815,3.858,3.849,3.81,3.756,3.695,3.63,3.305,3.018,2.771,2.56,2.218,1.957,1.751,1.585,1.449,1.335,1.239};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_7[i]*dens[j]*factor);}
G4double Ge_8[31]={1.053,1.192,1.462,1.72,1.966,2.194,2.401,2.586,2.751,3.362,3.758,4.034,4.227,4.451,4.545,4.568,4.549,4.507,4.452,4.39,4.057,3.744,3.468,3.226,2.825,2.511,2.258,2.052,1.881,1.737,1.615};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_8[i]*dens[j]*factor);}
G4double Ge_9[31]={1.111,1.254,1.535,1.806,2.068,2.317,2.549,2.76,2.951,3.677,4.159,4.504,4.754,5.063,5.213,5.272,5.278,5.251,5.206,5.15,4.816,4.486,4.186,3.918,3.466,3.102,2.806,2.561,2.355,2.181,2.031};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_9[i]*dens[j]*factor);}
G4double Ge_10[31]={1.169,1.316,1.605,1.887,2.16,2.424,2.675,2.91,3.125,3.961,4.53,4.945,5.254,5.655,5.868,5.969,6.003,5.996,5.963,5.916,5.592,5.249,4.93,4.64,4.141,3.732,3.394,3.11,2.87,2.664,2.487};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_10[i]*dens[j]*factor);}
G4double Ge_11[31]={1.224,1.375,1.671,1.96,2.242,2.518,2.784,3.038,3.275,4.216,4.872,5.358,5.729,6.228,6.511,6.662,6.73,6.746,6.73,6.693,6.388,6.039,5.705,5.396,4.855,4.404,4.024,3.704,3.428,3.191,2.984};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_11[i]*dens[j]*factor);}
G4double Ge_12[31]={1.281,1.438,1.739,2.035,2.325,2.608,2.885,3.154,3.409,4.448,5.187,5.742,6.174,6.772,7.129,7.334,7.44,7.484,7.486,7.463,7.181,6.829,6.482,6.158,5.58,5.091,4.674,4.318,4.01,3.743,3.508};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_12[i]*dens[j]*factor);}
G4double Ge_13[31]={1.335,1.498,1.806,2.106,2.402,2.692,2.977,3.256,3.525,4.654,5.477,6.101,6.593,7.293,7.728,7.991,8.14,8.215,8.241,8.235,7.987,7.637,7.282,6.945,6.335,5.81,5.358,4.968,4.628,4.33,4.068};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_13[i]*dens[j]*factor);}
G4double Ge_14[31]={1.389,1.558,1.873,2.178,2.478,2.773,3.064,3.351,3.631,4.841,5.744,6.438,6.99,7.791,8.305,8.628,8.823,8.931,8.982,8.995,8.789,8.446,8.086,7.738,7.1,6.543,6.059,5.637,5.267,4.941,4.651};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_14[i]*dens[j]*factor);}
G4double Ge_15[31]={1.437,1.612,1.936,2.244,2.548,2.848,3.143,3.435,3.723,5.006,5.988,6.75,7.363,8.27,8.868,9.258,9.504,9.652,9.733,9.768,9.621,9.294,8.935,8.58,7.918,7.331,6.815,6.361,5.959,5.603,5.286};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_15[i]*dens[j]*factor);}
G4double Ge_16[31]={1.485,1.667,2.002,2.315,2.622,2.927,3.227,3.523,3.818,5.162,6.218,7.047,7.721,8.733,9.419,9.878,1.18,1.37,1.48,1.54,1.46,1.15,9.797,9.438,8.756,8.143,7.598,7.113,6.682,6.297,5.952};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_16[i]*dens[j]*factor);}
G4double Ge_17[31]={1.533,1.721,2.066,2.384,2.695,3.002,3.305,3.605,3.903,5.3,6.424,7.319,8.05,9.166,9.94,1.47,1.83,11.06,11.21,11.29,11.28,11,1.66,1.3,9.599,8.963,8.39,7.878,7.419,7.007,6.636};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_17[i]*dens[j]*factor);}
G4double Ge_18[31]={1.585,1.779,2.137,2.463,2.778,3.088,3.395,3.698,4,5.439,6.629,7.587,8.375,9.592,1.45,11.05,11.47,11.74,11.93,12.04,12.1,11.84,11.5,11.14,1.42,9.764,9.167,8.629,8.145,7.708,7.313};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ge_18[i]*dens[j]*factor);}
j++;

G4double Kr_3[31]={.826,.9063,1.029,1.113,1.17,1.211,1.241,1.265,1.282,1.324,1.328,1.317,1.298,1.247,1.188,1.13,1.076,1.026,.9803,.9389,.7771,.6654,.5835,.521,.4315,.3705,.326,.292,.2652,.2433,.2252};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_3[i]*dens[j]*factor);}
G4double Kr_4[31]={.9936,1.106,1.292,1.432,1.534,1.611,1.671,1.72,1.759,1.872,1.917,1.931,1.929,1.894,1.838,1.775,1.71,1.647,1.588,1.532,1.304,1.135,1.007,.9061,.7574,.6533,.5763,.517,.4698,.4313,.3993};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_4[i]*dens[j]*factor);}
G4double Kr_5[31]={1.119,1.256,1.495,1.689,1.84,1.958,2.052,2.128,2.192,2.393,2.492,2.543,2.567,2.565,2.525,2.466,2.399,2.33,2.262,2.197,1.913,1.692,1.518,1.377,1.163,1.01,.8939,.8036,.7312,.6718,.6223};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_5[i]*dens[j]*factor);}
G4double Kr_6[31]={1.219,1.375,1.656,1.896,2.094,2.254,2.384,2.492,2.583,2.882,3.043,3.138,3.195,3.24,3.227,3.182,3.121,3.051,2.98,2.908,2.583,2.318,2.101,1.921,1.641,1.434,1.275,1.15,1.048,.9642,.8938};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_6[i]*dens[j]*factor);}
G4double Kr_7[31]={1.301,1.47,1.784,2.062,2.301,2.501,2.668,2.807,2.927,3.331,3.562,3.706,3.802,3.904,3.928,3.907,3.859,3.795,3.725,3.653,3.301,2.999,2.744,2.528,2.183,1.921,1.717,1.553,1.419,1.308,1.214};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_7[i]*dens[j]*factor);}
G4double Kr_8[31]={1.38,1.559,1.897,2.204,2.479,2.716,2.918,3.091,3.239,3.753,4.057,4.256,4.394,4.559,4.628,4.637,4.608,4.556,4.491,4.421,4.054,3.723,3.435,3.186,2.78,2.465,2.214,2.01,1.842,1.7,1.58};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_8[i]*dens[j]*factor);}
G4double Kr_9[31]={1.456,1.642,1.998,2.329,2.631,2.901,3.136,3.339,3.516,4.14,4.521,4.776,4.959,5.192,5.311,5.355,5.348,5.311,5.256,5.19,4.82,4.466,4.151,3.874,3.412,3.047,2.752,2.509,2.306,2.134,1.988};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_9[i]*dens[j]*factor);}
G4double Kr_10[31]={1.533,1.724,2.094,2.443,2.767,3.064,3.329,3.561,3.765,4.499,4.958,5.273,5.503,5.809,5.981,6.064,6.086,6.068,6.025,5.967,5.602,5.232,4.894,4.592,4.08,3.668,3.329,3.048,2.81,2.608,2.433};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_10[i]*dens[j]*factor);}
G4double Kr_11[31]={1.606,1.803,2.183,2.546,2.889,3.208,3.498,3.758,3.988,4.831,5.369,5.745,6.024,6.408,6.641,6.769,6.823,6.828,6.801,6.754,6.404,6.024,5.668,5.344,4.786,4.329,3.949,3.629,3.357,3.122,2.919};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_11[i]*dens[j]*factor);}
G4double Kr_12[31]={1.684,1.886,2.275,2.649,3.005,3.341,3.653,3.937,4.193,5.141,5.757,6.193,6.522,6.984,7.278,7.454,7.546,7.577,7.569,7.535,7.205,6.818,6.447,6.104,5.506,5.008,4.589,4.233,3.928,3.664,3.433};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_12[i]*dens[j]*factor);}
G4double Kr_13[31]={1.759,1.968,2.365,2.748,3.114,3.464,3.793,4.099,4.377,5.427,6.122,6.621,7,7.541,7.898,8.126,8.257,8.319,8.334,8.316,8.019,7.632,7.249,6.89,6.255,5.718,5.263,4.872,4.534,4.24,3.981};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_13[i]*dens[j]*factor);}
G4double Kr_14[31]={1.833,2.049,2.455,2.845,3.22,3.581,3.924,4.247,4.544,5.69,6.464,7.024,7.454,8.075,8.496,8.777,8.951,9.046,9.085,9.087,8.831,8.447,8.056,7.684,7.016,6.445,5.955,5.531,5.162,4.839,4.553};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_14[i]*dens[j]*factor);}
G4double Kr_15[31]={1.901,2.126,2.541,2.936,3.318,3.687,4.041,4.378,4.693,5.932,6.783,7.407,7.889,8.594,9.083,9.422,9.643,9.775,9.844,9.867,9.667,9.297,8.903,8.522,7.825,7.221,6.698,6.241,5.841,5.487,5.173};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_15[i]*dens[j]*factor);}
G4double Kr_16[31]={1.97,2.204,2.631,3.033,3.421,3.796,4.16,4.509,4.838,6.164,7.089,7.776,8.311,9.102,9.66,1.06,1.33,1.5,1.6,1.65,1.51,1.16,9.764,9.376,8.656,8.021,7.466,6.978,6.547,6.164,5.823};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_16[i]*dens[j]*factor);}
G4double Kr_17[31]={2.038,2.28,2.72,3.128,3.521,3.902,4.272,4.63,4.972,6.375,7.374,8.122,8.709,9.584,1.21,1.67,1.99,11.2,11.33,11.41,11.34,11.01,1.62,1.23,9.491,8.831,8.247,7.73,7.27,6.86,6.492};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_17[i]*dens[j]*factor);}
G4double Kr_18[31]={2.11,2.362,2.817,3.234,3.633,4.019,4.394,4.759,5.111,6.587,7.656,8.465,9.104,1.06,1.76,11.27,11.64,11.9,12.06,12.16,12.17,11.86,11.47,11.07,1.31,9.628,9.016,8.472,7.987,7.551,7.158};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Kr_18[i]*dens[j]*factor);}
j++;

G4double Mo_3[31]={.6534,.7272,.8512,.9467,1.019,1.073,1.114,1.147,1.173,1.235,1.244,1.236,1.22,1.177,1.13,1.083,1.037,.9929,.9519,.9138,.7605,.6532,.5743,.5137,.4269,.3673,.3238,.2904,.264,.2424,.2245};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_3[i]*dens[j]*factor);}
G4double Mo_4[31]={.7836,.8807,1.052,1.196,1.312,1.406,1.48,1.542,1.592,1.741,1.796,1.812,1.81,1.782,1.739,1.689,1.638,1.586,1.534,1.485,1.272,1.112,.9894,.8922,.7484,.6472,.5719,.5138,.4674,.4295,.3979};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_4[i]*dens[j]*factor);}
G4double Mo_5[31]={.8829,.9968,1.205,1.389,1.547,1.68,1.791,1.883,1.962,2.215,2.331,2.385,2.408,2.408,2.379,2.337,2.287,2.232,2.176,2.12,1.862,1.654,1.489,1.354,1.148,.9989,.8861,.7979,.7269,.6686,.6199};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_5[i]*dens[j]*factor);}
G4double Mo_6[31]={.9635,1.089,1.326,1.541,1.735,1.905,2.05,2.175,2.283,2.649,2.84,2.942,2.997,3.038,3.034,3.006,2.964,2.913,2.857,2.798,2.509,2.262,2.056,1.885,1.616,1.417,1.262,1.14,1.041,.9587,.8896};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_6[i]*dens[j]*factor);}
G4double Mo_7[31]={1.029,1.164,1.421,1.661,1.882,2.083,2.261,2.418,2.554,3.039,3.313,3.471,3.566,3.659,3.687,3.681,3.653,3.611,3.56,3.504,3.199,2.92,2.681,2.476,2.147,1.895,1.697,1.538,1.408,1.299,1.207};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_7[i]*dens[j]*factor);}
G4double Mo_8[31]={1.092,1.232,1.505,1.764,2.007,2.234,2.441,2.627,2.792,3.394,3.757,3.978,4.118,4.273,4.34,4.361,4.352,4.323,4.281,4.23,3.922,3.619,3.351,3.116,2.73,2.428,2.186,1.988,1.825,1.687,1.57};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_8[i]*dens[j]*factor);}
G4double Mo_9[31]={1.15,1.296,1.581,1.854,2.115,2.361,2.592,2.804,2.995,3.712,4.166,4.455,4.645,4.867,4.979,5.03,5.043,5.031,5,4.957,4.656,4.337,4.045,3.785,3.348,2.999,2.714,2.479,2.283,2.115,1.972};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_9[i]*dens[j]*factor);}
G4double Mo_10[31]={1.205,1.357,1.651,1.937,2.211,2.473,2.722,2.956,3.171,3.998,4.543,4.904,5.148,5.446,5.606,5.692,5.731,5.738,5.722,5.689,5.406,5.076,4.765,4.483,4,3.606,3.281,3.01,2.78,2.583,2.413};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_10[i]*dens[j]*factor);}
G4double Mo_11[31]={1.256,1.414,1.717,2.012,2.297,2.572,2.836,3.088,3.324,4.256,4.89,5.325,5.628,6.008,6.224,6.35,6.418,6.448,6.449,6.429,6.173,5.839,5.514,5.213,4.688,4.253,3.888,3.58,3.317,3.09,2.892};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_11[i]*dens[j]*factor);}
G4double Mo_12[31]={1.308,1.471,1.783,2.086,2.381,2.666,2.942,3.208,3.462,4.49,5.211,5.721,6.083,6.547,6.822,6.991,7.092,7.147,7.167,7.163,6.939,6.606,6.269,5.952,5.39,4.917,4.517,4.175,3.88,3.624,3.4};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_12[i]*dens[j]*factor);}
G4double Mo_13[31]={1.356,1.526,1.847,2.157,2.459,2.753,3.039,3.316,3.584,4.702,5.507,6.092,6.515,7.068,7.405,7.62,7.757,7.84,7.883,7.896,7.716,7.389,7.044,6.714,6.121,5.612,5.177,4.802,4.476,4.191,3.94};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_13[i]*dens[j]*factor);}
G4double Mo_14[31]={1.403,1.58,1.91,2.226,2.534,2.835,3.129,3.415,3.694,4.892,5.777,6.436,6.922,7.567,7.967,8.231,8.405,8.517,8.585,8.618,8.491,8.174,7.825,7.485,6.863,6.323,5.855,5.449,5.094,4.782,4.505};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_14[i]*dens[j]*factor);}
G4double Mo_15[31]={1.446,1.628,1.967,2.289,2.603,2.91,3.21,3.503,3.791,5.059,6.022,6.754,7.304,8.046,8.517,8.834,9.049,9.195,9.29,9.346,9.285,8.989,8.642,8.295,7.649,7.079,6.581,6.143,5.758,5.417,5.114};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_15[i]*dens[j]*factor);}
G4double Mo_16[31]={1.49,1.677,2.028,2.357,2.676,2.988,3.294,3.594,3.889,5.22,6.255,7.057,7.672,8.515,9.059,9.43,9.689,9.87,9.993,1.07,1.08,9.812,9.47,9.119,8.453,7.857,7.33,6.863,6.45,6.081,5.751};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_16[i]*dens[j]*factor);}
G4double Mo_17[31]={1.533,1.726,2.086,2.422,2.746,3.063,3.374,3.679,3.979,5.362,6.465,7.335,8.013,8.957,9.575,10,1.31,1.53,1.68,1.78,1.87,1.63,1.3,9.946,9.264,8.645,8.091,7.598,7.157,6.763,6.408};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_17[i]*dens[j]*factor);}
G4double Mo_18[31]={1.583,1.781,2.153,2.497,2.827,3.149,3.465,3.775,4.08,5.508,6.675,7.61,8.351,9.397,1.09,1.58,1.92,11.18,11.36,11.49,11.66,11.44,11.12,1.76,1.07,9.424,8.846,8.327,7.862,7.443,7.065};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Mo_18[i]*dens[j]*factor);}
j++;

G4double Ag_3[31]={.616,.695,.8256,.9274,1.006,1.064,1.108,1.141,1.165,1.222,1.224,1.207,1.184,1.131,1.079,1.031,.9856,.9444,.9066,.8719,.7337,.635,.5609,.5032,.4193,.3612,.3185,.2857,.2596,.2384,.2208};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_3[i]*dens[j]*factor);}
G4double Ag_4[31]={.7283,.8324,1.015,1.166,1.292,1.394,1.475,1.54,1.591,1.734,1.781,1.786,1.772,1.726,1.672,1.617,1.563,1.512,1.463,1.418,1.226,1.079,.9641,.8719,.7337,.6354,.5619,.505,.4595,.4223,.3913};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_4[i]*dens[j]*factor);}
G4double Ag_5[31]={.8167,.9362,1.158,1.352,1.52,1.664,1.785,1.886,1.969,2.219,2.326,2.367,2.374,2.348,2.3,2.246,2.19,2.133,2.079,2.026,1.792,1.602,1.448,1.32,1.123,.9791,.8694,.7834,.714,.657,.6092};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_5[i]*dens[j]*factor);}
G4double Ag_6[31]={.8963,1.023,1.271,1.499,1.702,1.884,2.043,2.18,2.297,2.669,2.85,2.937,2.974,2.98,2.948,2.901,2.847,2.79,2.733,2.676,2.413,2.188,1.998,1.836,1.579,1.387,1.237,1.118,1.021,.9412,.8736};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_6[i]*dens[j]*factor);}
G4double Ag_7[31]={.9709,1.099,1.362,1.615,1.847,2.06,2.252,2.424,2.574,3.076,3.34,3.483,3.558,3.608,3.6,3.567,3.521,3.468,3.412,3.354,3.074,2.822,2.601,2.409,2.095,1.853,1.661,1.507,1.38,1.274,1.184};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_7[i]*dens[j]*factor);}
G4double Ag_8[31]={1.047,1.177,1.447,1.716,1.972,2.21,2.431,2.632,2.814,3.45,3.804,4.01,4.129,4.233,4.256,4.241,4.207,4.161,4.109,4.054,3.767,3.495,3.248,3.029,2.661,2.37,2.136,1.945,1.786,1.653,1.538};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_8[i]*dens[j]*factor);}
G4double Ag_9[31]={1.12,1.254,1.527,1.807,2.08,2.339,2.582,2.809,3.019,3.786,4.234,4.508,4.676,4.843,4.902,4.91,4.891,4.855,4.809,4.758,4.472,4.186,3.92,3.677,3.262,2.926,2.651,2.424,2.233,2.071,1.931};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_9[i]*dens[j]*factor);}
G4double Ag_10[31]={1.189,1.331,1.607,1.893,2.178,2.453,2.715,2.963,3.196,4.09,4.633,4.979,5.202,5.44,5.541,5.576,5.574,5.551,5.514,5.468,5.192,4.898,4.616,4.354,3.896,3.517,3.204,2.941,2.718,2.527,2.362};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_10[i]*dens[j]*factor);}
G4double Ag_11[31]={1.251,1.402,1.685,1.974,2.267,2.555,2.832,3.098,3.35,4.362,5.003,5.424,5.705,6.023,6.173,6.24,6.26,6.252,6.226,6.189,5.928,5.632,5.338,5.06,4.563,4.145,3.794,3.496,3.241,3.021,2.829};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_11[i]*dens[j]*factor);}
G4double Ag_12[31]={1.311,1.472,1.766,2.058,2.356,2.653,2.942,3.221,3.49,4.609,5.348,5.844,6.185,6.586,6.788,6.892,6.937,6.947,6.934,6.906,6.666,6.37,6.068,5.776,5.246,4.792,4.406,4.075,3.789,3.541,3.324};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_12[i]*dens[j]*factor);}
G4double Ag_13[31]={1.366,1.537,1.845,2.14,2.441,2.744,3.043,3.334,3.615,4.83,5.666,6.24,6.644,7.133,7.392,7.535,7.608,7.638,7.641,7.624,7.413,7.124,6.817,6.515,5.956,5.468,5.048,4.685,4.37,4.094,3.851};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_13[i]*dens[j]*factor);}
G4double Ag_14[31]={1.419,1.597,1.922,2.221,2.525,2.832,3.138,3.438,3.73,5.028,5.958,6.61,7.077,7.659,7.978,8.163,8.266,8.319,8.338,8.336,8.16,7.88,7.572,7.262,6.678,6.16,5.709,5.316,4.972,4.67,4.402};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_14[i]*dens[j]*factor);}
G4double Ag_15[31]={1.465,1.65,1.99,2.298,2.603,2.912,3.223,3.53,3.831,5.199,6.222,6.951,7.483,8.164,8.55,8.782,8.92,8.999,9.039,9.052,8.924,8.662,8.357,8.043,7.439,6.894,6.413,5.99,5.617,5.287,4.993};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_15[i]*dens[j]*factor);}
G4double Ag_16[31]={1.511,1.703,2.059,2.377,2.685,2.997,3.312,3.625,3.933,5.361,6.471,7.277,7.875,8.659,9.115,9.397,9.572,9.678,9.74,9.771,9.692,9.452,9.154,8.839,8.218,7.648,7.14,6.689,6.289,5.932,5.613};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_16[i]*dens[j]*factor);}
G4double Ag_17[31]={1.555,1.753,2.123,2.454,2.767,3.08,3.397,3.714,4.028,5.505,6.696,7.578,8.242,9.129,9.659,9.994,1.21,1.34,1.43,1.48,1.45,1.24,9.951,9.637,9.004,8.413,7.88,7.403,6.976,6.595,6.251};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_17[i]*dens[j]*factor);}
G4double Ag_18[31]={1.605,1.81,2.193,2.539,2.858,3.175,3.494,3.814,4.133,5.653,6.919,7.875,8.603,9.596,1.2,1.59,1.84,11.01,11.12,11.18,11.21,11.02,1.74,1.43,9.784,9.172,8.615,8.113,7.663,7.258,6.892};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Ag_18[i]*dens[j]*factor);}
j++;

G4double Sn_3[31]={.5096,.5567,.632,.6872,.728,.7589,.7833,.8031,.8194,.8713,.8964,.9074,.9098,.8992,.8778,.8518,.8241,.7964,.7696,.744,.6366,.5568,.4957,.4473,.3757,.3253,.2879,.259,.2361,.2174,.2018};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_3[i]*dens[j]*factor);}
G4double Sn_4[31]={.6712,.7388,.8518,.9402,1.009,1.063,1.107,1.143,1.173,1.273,1.328,1.36,1.376,1.382,1.368,1.343,1.313,1.281,1.247,1.214,1.066,.9472,.8523,.7751,.6573,.5722,.508,.4579,.4179,.3851,.3577};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_4[i]*dens[j]*factor);}
G4double Sn_5[31]={.8182,.904,1.052,1.173,1.271,1.351,1.416,1.471,1.517,1.674,1.766,1.824,1.86,1.892,1.893,1.876,1.848,1.815,1.779,1.742,1.562,1.409,1.282,1.175,1.007,.8819,.7861,.7105,.6494,.5991,.5569};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_5[i]*dens[j]*factor);}
G4double Sn_6[31]={.9537,1.056,1.235,1.386,1.513,1.619,1.708,1.782,1.846,2.067,2.201,2.289,2.349,2.416,2.438,2.433,2.413,2.383,2.348,2.309,2.109,1.928,1.771,1.635,1.417,1.25,1.119,1.015,.9293,.8585,.7988};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_6[i]*dens[j]*factor);}
G4double Sn_7[31]={1.078,1.194,1.401,1.579,1.733,1.864,1.976,2.072,2.155,2.445,2.624,2.747,2.835,2.941,2.99,3.004,2.995,2.973,2.941,2.905,2.696,2.492,2.31,2.148,1.881,1.671,1.504,1.368,1.256,1.162,1.083};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_7[i]*dens[j]*factor);}
G4double Sn_8[31]={1.198,1.327,1.559,1.762,1.94,2.096,2.231,2.348,2.45,2.813,3.042,3.202,3.319,3.47,3.55,3.585,3.592,3.579,3.555,3.522,3.312,3.093,2.889,2.705,2.392,2.14,1.935,1.767,1.627,1.508,1.407};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_8[i]*dens[j]*factor);}
G4double Sn_9[31]={1.311,1.452,1.705,1.93,2.13,2.308,2.465,2.603,2.725,3.164,3.444,3.643,3.791,3.99,4.104,4.164,4.188,4.187,4.171,4.145,3.942,3.713,3.492,3.289,2.935,2.644,2.403,2.203,2.035,1.891,1.768};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_9[i]*dens[j]*factor);}
G4double Sn_10[31]={1.42,1.571,1.845,2.089,2.309,2.507,2.684,2.842,2.983,3.499,3.833,4.073,4.254,4.504,4.655,4.742,4.786,4.8,4.795,4.776,4.587,4.353,4.12,3.9,3.509,3.181,2.906,2.675,2.478,2.309,2.162};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_10[i]*dens[j]*factor);}
G4double Sn_11[31]={1.52,1.681,1.974,2.235,2.473,2.689,2.885,3.062,3.222,3.816,4.206,4.488,4.704,5.008,5.199,5.317,5.384,5.417,5.424,5.415,5.248,5.013,4.772,4.539,4.116,3.753,3.445,3.182,2.957,2.762,2.592};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_11[i]*dens[j]*factor);}
G4double Sn_12[31]={1.612,1.782,2.091,2.368,2.621,2.853,3.065,3.26,3.437,4.108,4.554,4.879,5.13,5.488,5.722,5.874,5.967,6.019,6.042,6.045,5.906,5.675,5.429,5.186,4.735,4.342,4.004,3.712,3.459,3.239,3.047};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_12[i]*dens[j]*factor);}
G4double Sn_13[31]={1.705,1.883,2.208,2.5,2.767,3.014,3.242,3.453,3.646,4.392,4.896,5.266,5.552,5.968,6.246,6.432,6.552,6.625,6.665,6.681,6.576,6.354,6.105,5.855,5.38,4.959,4.591,4.271,3.992,3.747,3.531};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_13[i]*dens[j]*factor);}
G4double Sn_14[31]={1.793,1.98,2.32,2.626,2.906,3.166,3.408,3.633,3.842,4.66,5.222,5.637,5.961,6.434,6.758,6.979,7.128,7.224,7.282,7.312,7.246,7.036,6.788,6.532,6.038,5.591,5.197,4.85,4.545,4.276,4.038};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_14[i]*dens[j]*factor);}
G4double Sn_15[31]={1.874,2.07,2.424,2.742,3.035,3.307,3.561,3.799,4.021,4.909,5.529,5.991,6.353,6.889,7.26,7.521,7.702,7.823,7.902,7.949,7.931,7.742,7.499,7.241,6.732,6.262,5.841,5.468,5.138,4.844,4.583};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_15[i]*dens[j]*factor);}
G4double Sn_16[31]={1.954,2.158,2.527,2.858,3.163,3.446,3.712,3.962,4.197,5.152,5.83,6.338,6.74,7.339,7.761,8.062,8.275,8.423,8.524,8.588,8.624,8.456,8.222,7.965,7.443,6.952,6.508,6.11,5.755,5.438,5.154};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_16[i]*dens[j]*factor);}
G4double Sn_17[31]={2.03,2.242,2.625,2.969,3.285,3.579,3.855,4.116,4.363,5.381,6.116,6.671,7.112,7.776,8.248,8.59,8.837,9.013,9.136,9.219,9.311,9.168,8.945,8.69,8.161,7.653,7.187,6.767,6.389,6.049,5.743};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_17[i]*dens[j]*factor);}
G4double Sn_18[31]={2.114,2.334,2.732,3.089,3.417,3.722,4.009,4.28,4.538,5.618,6.409,7.012,7.493,8.22,8.742,9.124,9.404,9.607,9.752,9.853,9.998,9.878,9.664,9.411,8.874,8.349,7.863,7.421,7.022,6.661,6.335};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Sn_18[i]*dens[j]*factor);}
j++;

G4double Xe_3[31]={.5617,.6027,.6687,.7189,.7574,.7872,.8104,.8285,.8426,.8788,.8879,.8864,.88,.8595,.8342,.807,.7794,.7523,.7263,.7016,.5984,.5227,.4653,.4202,.3538,.3069,.272,.245,.2235,.2058,.1912};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_3[i]*dens[j]*factor);}
G4double Xe_4[31]={.7468,.8068,.9059,.9845,1.048,1.099,1.14,1.174,1.202,1.285,1.32,1.334,1.337,1.327,1.305,1.277,1.246,1.213,1.18,1.147,1.003,.8898,.8004,.7282,.6187,.5396,.4798,.433,.3954,.3646,.3388};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_4[i]*dens[j]*factor);}
G4double Xe_5[31]={.9138,.9925,1.125,1.233,1.321,1.395,1.457,1.509,1.552,1.691,1.759,1.795,1.813,1.822,1.81,1.787,1.757,1.722,1.686,1.649,1.473,1.325,1.204,1.104,.9474,.8314,.7421,.6715,.6143,.567,.5273};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_5[i]*dens[j]*factor);}
G4double Xe_6[31]={1.065,1.162,1.327,1.463,1.578,1.675,1.758,1.829,1.889,2.09,2.197,2.259,2.297,2.332,2.336,2.322,2.297,2.265,2.229,2.19,1.991,1.814,1.664,1.537,1.333,1.178,1.056,.9584,.8786,.8122,.7561};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_6[i]*dens[j]*factor);}
G4double Xe_7[31]={1.201,1.315,1.51,1.675,1.814,1.935,2.039,2.129,2.207,2.475,2.625,2.717,2.778,2.845,2.871,2.872,2.856,2.829,2.796,2.758,2.547,2.348,2.172,2.02,1.769,1.574,1.418,1.292,1.187,1.099,1.025};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_7[i]*dens[j]*factor);}
G4double Xe_8[31]={1.329,1.459,1.685,1.875,2.04,2.183,2.307,2.417,2.513,2.851,3.048,3.174,3.26,3.364,3.414,3.432,3.429,3.411,3.382,3.347,3.133,2.916,2.719,2.544,2.25,2.015,1.825,1.668,1.537,1.426,1.331};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_8[i]*dens[j]*factor);}
G4double Xe_9[31]={1.448,1.593,1.845,2.061,2.249,2.413,2.559,2.687,2.801,3.212,3.458,3.619,3.732,3.875,3.953,3.991,4.002,3.994,3.973,3.943,3.733,3.504,3.29,3.095,2.762,2.49,2.266,2.079,1.922,1.787,1.672};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_9[i]*dens[j]*factor);}
G4double Xe_10[31]={1.56,1.718,1.997,2.236,2.446,2.631,2.796,2.944,3.075,3.558,3.856,4.055,4.197,4.382,4.49,4.55,4.578,4.583,4.571,4.548,4.348,4.112,3.883,3.673,3.304,2.997,2.741,2.524,2.34,2.182,2.045};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_10[i]*dens[j]*factor);}
G4double Xe_11[31]={1.664,1.834,2.136,2.398,2.629,2.834,3.018,3.183,3.332,3.888,4.239,4.478,4.65,4.881,5.022,5.108,5.155,5.175,5.175,5.16,4.979,4.74,4.501,4.276,3.876,3.536,3.249,3.003,2.792,2.609,2.45};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_11[i]*dens[j]*factor);}
G4double Xe_12[31]={1.759,1.94,2.263,2.545,2.795,3.019,3.22,3.402,3.567,4.195,4.6,4.879,5.082,5.359,5.535,5.649,5.718,5.755,5.768,5.764,5.607,5.37,5.125,4.889,4.461,4.093,3.776,3.503,3.267,3.061,2.88};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_12[i]*dens[j]*factor);}
G4double Xe_13[31]={1.854,2.046,2.389,2.69,2.958,3.199,3.418,3.616,3.797,4.495,4.955,5.276,5.512,5.838,6.05,6.192,6.284,6.339,6.367,6.375,6.248,6.017,5.767,5.522,5.071,4.675,4.331,4.031,3.77,3.54,3.337};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_13[i]*dens[j]*factor);}
G4double Xe_14[31]={1.946,2.147,2.508,2.828,3.113,3.371,3.605,3.819,4.015,4.781,5.295,5.659,5.929,6.305,6.555,6.726,6.842,6.916,6.959,6.979,6.889,6.667,6.416,6.165,5.693,5.273,4.903,4.578,4.293,4.04,3.817};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_14[i]*dens[j]*factor);}
G4double Xe_15[31]={2.032,2.241,2.619,2.955,3.257,3.53,3.779,4.008,4.218,5.049,5.619,6.026,6.331,6.762,7.052,7.256,7.398,7.495,7.557,7.592,7.545,7.339,7.091,6.837,6.348,5.905,5.511,5.161,4.852,4.577,4.331};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_15[i]*dens[j]*factor);}
G4double Xe_16[31]={2.116,2.335,2.729,3.08,3.397,3.686,3.949,4.192,4.416,5.311,5.935,6.386,6.728,7.214,7.547,7.784,7.954,8.073,8.154,8.206,8.206,8.02,7.778,7.522,7.021,6.557,6.14,5.767,5.435,5.137,4.87};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_16[i]*dens[j]*factor);}
G4double Xe_17[31]={2.199,2.425,2.835,3.2,3.532,3.834,4.111,4.367,4.604,5.561,6.238,6.733,7.111,7.655,8.03,8.302,8.5,8.643,8.744,8.812,8.864,8.699,8.465,8.211,7.7,7.22,6.782,6.387,6.033,5.714,5.427};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_17[i]*dens[j]*factor);}
G4double Xe_18[31]={2.288,2.523,2.948,3.329,3.675,3.991,4.282,4.551,4.801,5.818,6.548,7.088,7.503,8.104,8.522,8.826,9.052,9.217,9.337,9.421,9.521,9.377,9.15,8.896,8.375,7.879,7.422,7.007,6.632,6.293,5.987};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Xe_18[i]*dens[j]*factor);}
j++;

G4double W_3[31]={.2439,.2673,.3038,.3307,.3513,.368,.3819,.3938,.4041,.4399,.4601,.4714,.4774,.4805,.4778,.4725,.4657,.4583,.4506,.4428,.4054,.3722,.3432,.3182,.2778,.2471,.2231,.2039,.1882,.1751,.164};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_3[i]*dens[j]*factor);}
G4double W_4[31]={.3226,.3566,.4129,.4564,.4909,.5191,.543,.5637,.5818,.648,.6891,.7156,.7327,.7499,.7556,.7553,.7512,.7447,.7368,.7281,.6801,.6322,.5883,.5491,.4837,.4326,.392,.3592,.332,.3093,.2899};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_4[i]*dens[j]*factor);}
G4double W_5[31]={.3961,.4392,.514,.5747,.6241,.6653,.7005,.7311,.7582,.8591,.925,.9703,1.002,1.04,1.059,1.067,1.069,1.066,1.06,1.052,1,.9409,.8833,.8299,.7381,.6641,.6042,.5551,.5141,.4795,.4499};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_5[i]*dens[j]*factor);}
G4double W_6[31]={.466,.5168,.608,.6852,.7498,.8046,.8517,.893,.9298,1.069,1.162,1.229,1.278,1.342,1.379,1.399,1.409,1.412,1.41,1.405,1.355,1.289,1.22,1.154,1.036,.9378,.8569,.7896,.7329,.6847,.6431};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_6[i]*dens[j]*factor);}
G4double W_7[31]={.5322,.5896,.695,.7871,.8664,.9347,.9941,1.047,1.093,1.273,1.396,1.487,1.556,1.65,1.708,1.743,1.764,1.775,1.779,1.779,1.738,1.669,1.592,1.514,1.372,1.25,1.147,1.061,.9871,.9237,.8687};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_7[i]*dens[j]*factor);}
G4double W_8[31]={.6002,.664,.7818,.8875,.9807,1.062,1.134,1.198,1.255,1.477,1.632,1.748,1.837,1.964,2.046,2.099,2.132,2.153,2.165,2.17,2.144,2.075,1.993,1.906,1.742,1.597,1.473,1.366,1.274,1.195,1.125};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_8[i]*dens[j]*factor);}
G4double W_9[31]={.6666,.7365,.8655,.9831,1.089,1.183,1.267,1.342,1.41,1.674,1.862,2.004,2.116,2.277,2.384,2.456,2.504,2.536,2.557,2.569,2.561,2.497,2.412,2.319,2.136,1.97,1.825,1.699,1.59,1.494,1.41};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_9[i]*dens[j]*factor);}
G4double W_10[31]={.7327,.8086,.9483,1.076,1.194,1.3,1.395,1.481,1.559,1.867,2.088,2.258,2.392,2.59,2.724,2.817,2.881,2.926,2.956,2.977,2.992,2.936,2.85,2.753,2.554,2.369,2.205,2.06,1.932,1.82,1.721};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_10[i]*dens[j]*factor);}
G4double W_11[31]={.7964,.8785,1.028,1.166,1.294,1.411,1.517,1.614,1.702,2.053,2.308,2.506,2.663,2.898,3.062,3.178,3.261,3.32,3.362,3.392,3.435,3.39,3.307,3.207,2.995,2.793,2.61,2.446,2.301,2.173,2.058};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_11[i]*dens[j]*factor);}
G4double W_12[31]={.8567,.9447,1.104,1.251,1.387,1.514,1.63,1.737,1.835,2.228,2.517,2.742,2.923,3.195,3.389,3.531,3.634,3.709,3.764,3.805,3.879,3.848,3.769,3.669,3.448,3.231,3.031,2.851,2.689,2.545,2.415};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_12[i]*dens[j]*factor);}
G4double W_13[31]={.9191,1.013,1.183,1.338,1.483,1.618,1.744,1.86,1.968,2.402,2.725,2.979,3.184,3.496,3.721,3.888,4.012,4.103,4.172,4.224,4.333,4.318,4.247,4.148,3.92,3.689,3.474,3.277,3.1,2.94,2.795};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_13[i]*dens[j]*factor);}
G4double W_14[31]={.9809,1.081,1.262,1.425,1.577,1.721,1.856,1.981,2.097,2.572,2.928,3.21,3.439,3.791,4.049,4.242,4.386,4.496,4.578,4.642,4.79,4.794,4.732,4.636,4.403,4.161,3.931,3.719,3.527,3.352,3.193};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_14[i]*dens[j]*factor);}
G4double W_15[31]={1.039,1.146,1.336,1.508,1.667,1.818,1.96,2.094,2.218,2.732,3.121,3.431,3.686,4.079,4.371,4.592,4.761,4.889,4.988,5.065,5.257,5.285,5.234,5.144,4.91,4.658,4.415,4.189,3.981,3.791,3.618};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_15[i]*dens[j]*factor);}
G4double W_16[31]={1.098,1.211,1.412,1.592,1.759,1.917,2.067,2.208,2.34,2.893,3.314,3.653,3.932,4.368,4.695,4.945,5.137,5.286,5.401,5.492,5.731,5.783,5.747,5.663,5.43,5.171,4.916,4.676,4.454,4.25,4.062};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_16[i]*dens[j]*factor);}
G4double W_17[31]={1.154,1.273,1.486,1.674,1.848,2.013,2.169,2.317,2.457,3.046,3.499,3.865,4.169,4.649,5.01,5.29,5.507,5.676,5.809,5.915,6.202,6.282,6.262,6.186,5.957,5.692,5.428,5.175,4.94,4.722,4.521};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_17[i]*dens[j]*factor);}
G4double W_18[31]={1.219,1.344,1.569,1.767,1.949,2.12,2.283,2.438,2.586,3.211,3.696,4.09,4.419,4.94,5.336,5.643,5.884,6.073,6.223,6.343,6.677,6.782,6.776,6.709,6.484,6.215,5.94,5.677,5.429,5.199,4.985};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],W_18[i]*dens[j]*factor);}
j++;

G4double Pt_3[31]={.2992,.3235,.3633,.3948,.4203,.4415,.4596,.4752,.4888,.5363,.5617,.5743,.5791,.5764,.5666,.5541,.5405,.527,.5137,.5011,.446,.4022,.3663,.3365,.2897,.2551,.2285,.2074,.1904,.1763,.1644};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_3[i]*dens[j]*factor);}
G4double Pt_4[31]={.4136,.4491,.5084,.5565,.5962,.6298,.6587,.6839,.7063,.7881,.8373,.8667,.8831,.8938,.8903,.8805,.8672,.8523,.8367,.821,.7467,.6824,.6274,.5802,.5043,.4465,.4015,.3655,.336,.3115,.2908};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_4[i]*dens[j]*factor);}
G4double Pt_5[31]={.5215,.568,.6469,.712,.7668,.8137,.8544,.8903,.9224,1.042,1.12,1.17,1.202,1.233,1.241,1.238,1.228,1.214,1.199,1.182,1.095,1.014,.9405,.8758,.7687,.6851,.6186,.5648,.5204,.4832,.4516};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_5[i]*dens[j]*factor);}
G4double Pt_6[31]={.6218,.679,.7771,.8591,.9292,.9899,1.043,1.09,1.132,1.294,1.402,1.475,1.526,1.584,1.608,1.615,1.611,1.601,1.588,1.572,1.48,1.385,1.296,1.215,1.077,.9665,.8768,.8032,.7419,.6901,.6458};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_6[i]*dens[j]*factor);}
G4double Pt_7[31]={.7138,.781,.8974,.9958,1.081,1.155,1.221,1.28,1.333,1.538,1.679,1.779,1.851,1.939,1.984,2.004,2.01,2.006,1.997,1.984,1.893,1.79,1.688,1.593,1.425,1.287,1.173,1.079,.9988,.931,.8725};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_7[i]*dens[j]*factor);}
G4double Pt_8[31]={.803,.8796,1.014,1.128,1.228,1.316,1.395,1.466,1.529,1.779,1.955,2.083,2.177,2.3,2.368,2.404,2.42,2.424,2.421,2.412,2.328,2.22,2.109,2.001,1.807,1.642,1.504,1.388,1.289,1.204,1.13};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_8[i]*dens[j]*factor);}
G4double Pt_9[31]={.8862,.9718,1.122,1.252,1.366,1.468,1.559,1.641,1.716,2.012,2.223,2.38,2.499,2.658,2.751,2.804,2.834,2.847,2.851,2.847,2.775,2.666,2.549,2.431,2.214,2.025,1.863,1.726,1.608,1.505,1.416};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_9[i]*dens[j]*factor);}
G4double Pt_10[31]={.9654,1.059,1.226,1.37,1.498,1.612,1.716,1.81,1.896,2.238,2.485,2.672,2.816,3.014,3.134,3.207,3.251,3.275,3.287,3.29,3.234,3.128,3.007,2.882,2.644,2.432,2.249,2.091,1.953,1.833,1.728};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_10[i]*dens[j]*factor);}
G4double Pt_11[31]={1.04,1.141,1.322,1.481,1.621,1.748,1.864,1.969,2.066,2.455,2.74,2.957,3.127,3.365,3.515,3.61,3.671,3.708,3.729,3.739,3.704,3.605,3.482,3.352,3.096,2.864,2.66,2.481,2.325,2.187,2.066};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_11[i]*dens[j]*factor);}
G4double Pt_12[31]={1.108,1.217,1.411,1.582,1.735,1.874,2,2.116,2.224,2.658,2.98,3.228,3.424,3.702,3.884,4.004,4.083,4.134,4.166,4.185,4.175,4.085,3.964,3.83,3.561,3.311,3.088,2.89,2.716,2.561,2.424};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_12[i]*dens[j]*factor);}
G4double Pt_13[31]={1.176,1.292,1.499,1.683,1.847,1.997,2.135,2.262,2.379,2.86,3.218,3.498,3.72,4.042,4.255,4.4,4.498,4.564,4.608,4.637,4.655,4.577,4.46,4.325,4.044,3.778,3.537,3.321,3.129,2.958,2.804};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_13[i]*dens[j]*factor);}
G4double Pt_14[31]={1.242,1.365,1.585,1.779,1.955,2.116,2.264,2.401,2.528,3.053,3.448,3.759,4.008,4.375,4.622,4.792,4.91,4.992,5.048,5.087,5.137,5.073,4.961,4.827,4.538,4.258,4,3.767,3.559,3.371,3.203};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_14[i]*dens[j]*factor);}
G4double Pt_15[31]={1.303,1.432,1.663,1.869,2.055,2.226,2.383,2.53,2.667,3.235,3.666,4.008,4.285,4.698,4.981,5.18,5.32,5.419,5.49,5.541,5.628,5.583,5.48,5.349,5.055,4.762,4.489,4.24,4.015,3.811,3.627};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_15[i]*dens[j]*factor);}
G4double Pt_16[31]={1.363,1.498,1.741,1.957,2.154,2.334,2.501,2.657,2.804,3.414,3.882,4.255,4.56,5.019,5.34,5.568,5.731,5.849,5.935,5.998,6.124,6.1,6.008,5.88,5.584,5.281,4.994,4.73,4.489,4.27,4.071};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_16[i]*dens[j]*factor);}
G4double Pt_17[31]={1.42,1.561,1.815,2.041,2.247,2.437,2.613,2.778,2.933,3.584,4.087,4.492,4.824,5.331,5.69,5.947,6.134,6.271,6.373,6.449,6.618,6.616,6.537,6.416,6.121,5.809,5.51,5.232,4.977,4.743,4.53};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_17[i]*dens[j]*factor);}
G4double Pt_18[31]={1.484,1.632,1.897,2.133,2.349,2.548,2.733,2.907,3.071,3.763,4.302,4.738,5.097,5.651,6.047,6.333,6.543,6.699,6.816,6.905,7.113,7.133,7.065,6.95,6.656,6.338,6.028,5.737,5.468,5.221,4.994};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pt_18[i]*dens[j]*factor);}
j++;

G4double Au_3[31]={.3146,.3401,.3821,.4151,.4417,.4636,.4822,.4981,.512,.5601,.5855,.5977,.6018,.5976,.5859,.5715,.5563,.5412,.5266,.5127,.4536,.4077,.3708,.3403,.2927,.2576,.2306,.2093,.192,.1778,.1657};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_3[i]*dens[j]*factor);}
G4double Au_4[31]={.4344,.4717,.5341,.5845,.6261,.6609,.6907,.7167,.7395,.8225,.8719,.9009,.9167,.9255,.9198,.9076,.8921,.875,.8575,.84,.7597,.6921,.6352,.5868,.5095,.4509,.4052,.3687,.3389,.3142,.2932};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_4[i]*dens[j]*factor);}
G4double Au_5[31]={.5472,.5962,.6791,.7473,.8046,.8534,.8955,.9324,.9652,1.087,1.165,1.215,1.246,1.276,1.281,1.275,1.263,1.246,1.228,1.209,1.114,1.028,.9523,.8859,.7766,.6917,.6243,.5698,.5249,.4873,.4553};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_5[i]*dens[j]*factor);}
G4double Au_6[31]={.6522,.7123,.8152,.9011,.9743,1.037,1.093,1.141,1.185,1.349,1.457,1.531,1.581,1.637,1.659,1.663,1.656,1.643,1.627,1.608,1.506,1.405,1.313,1.23,1.088,.9758,.8848,.8102,.7482,.6958,.651};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_6[i]*dens[j]*factor);}
G4double Au_7[31]={.7484,.8189,.9409,1.044,1.133,1.21,1.279,1.339,1.394,1.602,1.744,1.844,1.916,2.003,2.045,2.062,2.064,2.057,2.045,2.029,1.926,1.816,1.71,1.611,1.44,1.299,1.184,1.088,1.007,.9385,.8794};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_7[i]*dens[j]*factor);}
G4double Au_8[31]={.8412,.9217,1.062,1.182,1.286,1.378,1.459,1.532,1.598,1.853,2.03,2.158,2.253,2.374,2.439,2.472,2.485,2.486,2.479,2.466,2.369,2.253,2.136,2.025,1.826,1.658,1.518,1.4,1.3,1.214,1.139};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_8[i]*dens[j]*factor);}
G4double Au_9[31]={.9278,1.018,1.175,1.311,1.43,1.536,1.63,1.715,1.793,2.094,2.307,2.465,2.584,2.742,2.833,2.883,2.909,2.919,2.919,2.911,2.824,2.706,2.582,2.46,2.237,2.044,1.88,1.74,1.621,1.517,1.427};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_9[i]*dens[j]*factor);}
G4double Au_10[31]={1.01,1.109,1.283,1.433,1.567,1.686,1.793,1.89,1.979,2.329,2.579,2.767,2.911,3.108,3.227,3.297,3.337,3.357,3.365,3.364,3.291,3.175,3.046,2.916,2.671,2.456,2.269,2.108,1.969,1.848,1.741};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_10[i]*dens[j]*factor);}
G4double Au_11[31]={1.087,1.194,1.383,1.548,1.695,1.827,1.946,2.056,2.156,2.553,2.842,3.061,3.231,3.469,3.617,3.71,3.766,3.799,3.817,3.823,3.77,3.659,3.528,3.392,3.129,2.892,2.684,2.502,2.344,2.204,2.081};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_11[i]*dens[j]*factor);}
G4double Au_12[31]={1.158,1.272,1.475,1.653,1.813,1.957,2.088,2.208,2.319,2.764,3.09,3.341,3.537,3.816,3.996,4.113,4.188,4.236,4.264,4.279,4.25,4.147,4.016,3.877,3.599,3.343,3.116,2.915,2.738,2.581,2.442};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_12[i]*dens[j]*factor);}
G4double Au_13[31]={1.228,1.349,1.566,1.757,1.929,2.084,2.227,2.358,2.48,2.972,3.336,3.618,3.842,4.165,4.377,4.519,4.614,4.676,4.716,4.74,4.739,4.646,4.519,4.377,4.087,3.815,3.568,3.349,3.154,2.981,2.825};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_13[i]*dens[j]*factor);}
G4double Au_14[31]={1.296,1.424,1.653,1.856,2.039,2.206,2.36,2.502,2.633,3.171,3.573,3.887,4.139,4.506,4.753,4.921,5.035,5.113,5.165,5.2,5.229,5.15,5.028,4.886,4.587,4.299,4.036,3.8,3.588,3.398,3.227};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_14[i]*dens[j]*factor);}
G4double Au_15[31]={1.358,1.493,1.734,1.949,2.142,2.32,2.483,2.635,2.776,3.359,3.798,4.145,4.424,4.838,5.122,5.318,5.455,5.551,5.617,5.663,5.729,5.668,5.554,5.414,5.109,4.809,4.53,4.276,4.047,3.841,3.654};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_15[i]*dens[j]*factor);}
G4double Au_16[31]={1.42,1.561,1.814,2.039,2.244,2.431,2.605,2.766,2.917,3.544,4.021,4.399,4.706,5.168,5.489,5.715,5.875,5.989,6.071,6.13,6.234,6.192,6.088,5.952,5.645,5.333,5.04,4.771,4.526,4.303,4.101};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_16[i]*dens[j]*factor);}
G4double Au_17[31]={1.479,1.626,1.89,2.125,2.339,2.536,2.719,2.89,3.05,3.719,4.232,4.642,4.978,5.489,5.848,6.104,6.288,6.422,6.519,6.591,6.737,6.717,6.624,6.494,6.187,5.867,5.561,5.277,5.017,4.78,4.563};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_17[i]*dens[j]*factor);}
G4double Au_18[31]={1.543,1.697,1.973,2.219,2.443,2.65,2.842,3.022,3.191,3.903,4.453,4.895,5.259,5.817,6.213,6.499,6.706,6.859,6.971,7.056,7.241,7.241,7.16,7.036,6.729,6.401,6.083,5.787,5.513,5.261,5.031};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Au_18[i]*dens[j]*factor);}
j++;

G4double Pb_3[31]={.3483,.3786,.426,.4605,.4861,.5059,.5217,.5346,.5454,.5801,.5962,.602,.6016,.5907,.5742,.5559,.5375,.5197,.5029,.4873,.4243,.3787,.3436,.3154,.2722,.2406,.2163,.1971,.1814,.1684,.1574};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_3[i]*dens[j]*factor);}
G4double Pb_4[31]={.4635,.5078,.5801,.6356,.6786,.7126,.7402,.7631,.7826,.8488,.8853,.9051,.9142,.9137,.9012,.8835,.8633,.8423,.8215,.8013,.714,.646,.5913,.5461,.4751,.4219,.3805,.3473,.3202,.2976,.2784};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_4[i]*dens[j]*factor);}
G4double Pb_5[31]={.5692,.6265,.7228,.7996,.8612,.911,.9521,.9866,1.016,1.119,1.18,1.218,1.241,1.258,1.255,1.242,1.224,1.202,1.18,1.157,1.052,.9641,.8904,.8276,.7265,.6487,.5871,.5373,.4962,.4617,.4323};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_5[i]*dens[j]*factor);}
G4double Pb_6[31]={.6665,.7355,.8541,.9515,1.032,1.098,1.154,1.201,1.242,1.386,1.475,1.534,1.573,1.614,1.626,1.621,1.607,1.588,1.566,1.543,1.427,1.323,1.232,1.153,1.021,.9172,.8336,.765,.7079,.6597,.6184};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_6[i]*dens[j]*factor);}
G4double Pb_7[31]={.7558,.8351,.9739,1.091,1.189,1.272,1.343,1.403,1.456,1.645,1.765,1.848,1.906,1.974,2.005,2.012,2.006,1.992,1.973,1.952,1.831,1.715,1.61,1.515,1.354,1.224,1.117,1.029,.954,.8905,.8358};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_7[i]*dens[j]*factor);}
G4double Pb_8[31]={.8429,.9317,1.089,1.223,1.339,1.439,1.524,1.599,1.664,1.9,2.053,2.161,2.24,2.34,2.391,2.414,2.418,2.411,2.396,2.378,2.259,2.135,2.018,1.91,1.722,1.565,1.435,1.325,1.232,1.153,1.083};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_8[i]*dens[j]*factor);}
G4double Pb_9[31]={.9254,1.023,1.197,1.348,1.479,1.594,1.695,1.783,1.86,2.146,2.334,2.469,2.57,2.703,2.778,2.817,2.833,2.835,2.827,2.812,2.7,2.571,2.445,2.327,2.114,1.934,1.781,1.651,1.539,1.442,1.358};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_9[i]*dens[j]*factor);}
G4double Pb_10[31]={1.005,1.11,1.299,1.465,1.612,1.742,1.856,1.958,2.048,2.384,2.609,2.772,2.896,3.065,3.166,3.224,3.253,3.265,3.264,3.255,3.154,3.024,2.892,2.765,2.531,2.327,2.153,2.002,1.872,1.758,1.658};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_10[i]*dens[j]*factor);}
G4double Pb_11[31]={1.081,1.193,1.396,1.576,1.736,1.88,2.008,2.123,2.226,2.613,2.875,3.068,3.216,3.422,3.551,3.63,3.676,3.699,3.708,3.706,3.621,3.493,3.358,3.223,2.97,2.746,2.55,2.38,2.231,2.1,1.984};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_11[i]*dens[j]*factor);}
G4double Pb_12[31]={1.152,1.271,1.487,1.679,1.851,2.007,2.148,2.275,2.39,2.828,3.128,3.351,3.523,3.767,3.925,4.028,4.092,4.129,4.148,4.154,4.09,3.968,3.831,3.691,3.422,3.18,2.965,2.776,2.609,2.461,2.33};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_12[i]*dens[j]*factor);}
G4double Pb_13[31]={1.223,1.349,1.577,1.781,1.965,2.133,2.285,2.424,2.551,3.04,3.378,3.632,3.83,4.114,4.303,4.429,4.512,4.563,4.593,4.608,4.569,4.455,4.318,4.175,3.894,3.634,3.401,3.193,3.009,2.845,2.698};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_13[i]*dens[j]*factor);}
G4double Pb_14[31]={1.293,1.426,1.666,1.88,2.075,2.253,2.417,2.567,2.705,3.243,3.621,3.905,4.129,4.455,4.675,4.827,4.928,4.995,5.038,5.062,5.051,4.947,4.813,4.669,4.376,4.101,3.851,3.627,3.426,3.246,3.084};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_14[i]*dens[j]*factor);}
G4double Pb_15[31]={1.358,1.497,1.748,1.973,2.178,2.366,2.54,2.7,2.848,3.435,3.851,4.167,4.417,4.787,5.042,5.22,5.344,5.428,5.484,5.52,5.543,5.454,5.326,5.181,4.882,4.594,4.328,4.087,3.869,3.673,3.496};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_15[i]*dens[j]*factor);}
G4double Pb_16[31]={1.423,1.569,1.831,2.065,2.28,2.477,2.661,2.831,2.989,3.624,4.078,4.426,4.703,5.118,5.408,5.614,5.76,5.863,5.934,5.982,6.04,5.968,5.847,5.705,5.401,5.101,4.82,4.564,4.33,4.119,3.927};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_16[i]*dens[j]*factor);}
G4double Pb_17[31]={1.485,1.637,1.91,2.154,2.377,2.584,2.776,2.955,3.122,3.802,4.296,4.675,4.98,5.44,5.765,6,6.17,6.291,6.378,6.439,6.536,6.483,6.371,6.233,5.927,5.617,5.324,5.053,4.805,4.579,4.372};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_17[i]*dens[j]*factor);}
G4double Pb_18[31]={1.554,1.713,1.998,2.252,2.485,2.7,2.901,3.089,3.265,3.99,4.522,4.934,5.266,5.771,6.132,6.394,6.586,6.726,6.827,6.901,7.035,6.999,6.897,6.762,6.454,6.136,5.831,5.547,5.285,5.044,4.824};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],Pb_18[i]*dens[j]*factor);}
j++;

G4double U_3[31]={.4158,.4488,.4987,.5342,.5603,.5798,.5944,.6054,.6139,.6323,.6313,.623,.6119,.5877,.564,.5413,.5196,.4992,.4801,.4623,.3915,.3427,.3074,.2806,.2419,.2148,.1943,.1781,.1649,.1538,.1443};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_3[i]*dens[j]*factor);}
G4double U_4[31]={.5309,.5804,.6584,.7166,.7614,.7965,.8243,.8465,.8645,.9152,.9314,.9324,.9264,.9057,.8816,.8569,.8318,.8068,.7824,.7591,.6601,.5867,.5313,.4878,.4235,.3773,.3421,.314,.291,.2716,.2551};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_4[i]*dens[j]*factor);}
G4double U_5[31]={.6283,.6928,.7987,.8807,.9455,.9979,1.041,1.076,1.105,1.196,1.235,1.25,1.253,1.243,1.224,1.201,1.175,1.149,1.122,1.095,.9736,.8778,.8024,.7416,.6491,.581,.5283,.4859,.4508,.4212,.3959};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_5[i]*dens[j]*factor);}
G4double U_6[31]={.7126,.79,.9217,1.027,1.112,1.183,1.241,1.291,1.332,1.468,1.535,1.569,1.585,1.59,1.58,1.563,1.54,1.514,1.486,1.458,1.322,1.207,1.113,1.036,.9142,.8227,.7506,.692,.6431,.6017,.566};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_6[i]*dens[j]*factor);}
G4double U_7[31]={.786,.8741,1.029,1.156,1.261,1.35,1.425,1.489,1.543,1.728,1.828,1.883,1.914,1.941,1.944,1.935,1.917,1.895,1.869,1.841,1.697,1.567,1.457,1.364,1.215,1.099,1.007,.9307,.8667,.812,.7647};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_7[i]*dens[j]*factor);}
G4double U_8[31]={.8561,.9532,1.127,1.275,1.4,1.506,1.597,1.676,1.744,1.982,2.116,2.196,2.245,2.296,2.314,2.316,2.306,2.289,2.267,2.241,2.094,1.953,1.829,1.722,1.546,1.407,1.294,1.2,1.12,1.051,.9908};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_8[i]*dens[j]*factor);}
G4double U_9[31]={.9213,1.026,1.217,1.383,1.526,1.649,1.756,1.849,1.932,2.223,2.394,2.501,2.57,2.648,2.684,2.698,2.698,2.688,2.67,2.648,2.504,2.355,2.22,2.101,1.902,1.74,1.607,1.495,1.398,1.315,1.242};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_9[i]*dens[j]*factor);}
G4double U_10[31]={.9839,1.096,1.3,1.483,1.642,1.782,1.904,2.012,2.107,2.453,2.664,2.8,2.891,2.999,3.055,3.083,3.094,3.092,3.08,3.063,2.927,2.773,2.629,2.5,2.279,2.097,1.944,1.814,1.701,1.603,1.516};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_10[i]*dens[j]*factor);}
G4double U_11[31]={1.043,1.161,1.378,1.574,1.749,1.903,2.04,2.162,2.271,2.672,2.924,3.09,3.204,3.344,3.422,3.468,3.491,3.499,3.496,3.484,3.361,3.206,3.056,2.918,2.677,2.476,2.304,2.157,2.028,1.915,1.814};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_11[i]*dens[j]*factor);}
G4double U_12[31]={1.098,1.222,1.45,1.657,1.845,2.013,2.164,2.298,2.42,2.875,3.168,3.365,3.503,3.677,3.78,3.844,3.883,3.902,3.908,3.903,3.798,3.645,3.49,3.345,3.088,2.87,2.681,2.517,2.372,2.244,2.131};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_12[i]*dens[j]*factor);}
G4double U_13[31]={1.155,1.284,1.522,1.74,1.939,2.12,2.283,2.431,2.564,3.074,3.408,3.637,3.801,4.011,4.14,4.224,4.277,4.309,4.324,4.328,4.244,4.096,3.939,3.788,3.517,3.282,3.077,2.896,2.737,2.595,2.467};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_13[i]*dens[j]*factor);}
G4double U_14[31]={1.209,1.345,1.592,1.819,2.029,2.221,2.396,2.555,2.7,3.262,3.638,3.9,4.09,4.34,4.495,4.599,4.669,4.713,4.739,4.751,4.692,4.551,4.394,4.24,3.956,3.707,3.486,3.291,3.117,2.961,2.82};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_14[i]*dens[j]*factor);}
G4double U_15[31]={1.261,1.402,1.658,1.893,2.111,2.313,2.499,2.669,2.825,3.437,3.854,4.15,4.367,4.658,4.844,4.971,5.058,5.118,5.156,5.178,5.149,5.02,4.865,4.709,4.416,4.153,3.919,3.709,3.521,3.351,3.197};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_15[i]*dens[j]*factor);}
G4double U_16[31]={1.313,1.459,1.725,1.967,2.193,2.404,2.599,2.78,2.946,3.607,4.065,4.396,4.641,4.975,5.192,5.342,5.449,5.524,5.575,5.608,5.611,5.495,5.345,5.189,4.888,4.614,4.367,4.143,3.941,3.757,3.59};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_16[i]*dens[j]*factor);}
G4double U_17[31]={1.363,1.514,1.789,2.038,2.271,2.49,2.694,2.884,3.06,3.767,4.266,4.63,4.904,5.283,5.532,5.707,5.833,5.924,5.989,6.033,6.073,5.972,5.828,5.673,5.368,5.084,4.825,4.589,4.373,4.177,3.998};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_17[i]*dens[j]*factor);}
G4double U_18[31]={1.419,1.577,1.861,2.118,2.358,2.585,2.797,2.996,3.181,3.935,4.474,4.872,5.175,5.599,5.879,6.078,6.223,6.33,6.408,6.463,6.537,6.451,6.313,6.159,5.849,5.556,5.286,5.038,4.811,4.603,4.412};
pv=new G4LPhysicsFreeVector(31,E[0],E[30]);pv->SetSpline(spline);dedx.push_back(pv);
for(i=0;i<31;i++){pv->PutValues(i,E[i],U_18[i]*dens[j]*factor);}
j++;



if(corr) {
  G4int n = 0;
  for(j=0; j<25; j++) {
    for(i=0; i<16; i++) {
      corr->AddStoppingData(Z[i], Z[i], NameMaterial[j], dedx[n]);
      n++;
    }
  }
}
}
