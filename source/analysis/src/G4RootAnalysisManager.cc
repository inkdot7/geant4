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

// Author: Ivana Hrivnacova, 15/06/2011  (ivana@ipno.in2p3.fr)

#include "G4RootAnalysisManager.hh"
#include "G4UnitsTable.hh"

#include <iostream>

G4RootAnalysisManager* G4RootAnalysisManager::fgInstance = 0;

//_____________________________________________________________________________
G4RootAnalysisManager* G4RootAnalysisManager::Instance()
{
  if ( fgInstance == 0 ) {
    fgInstance = new G4RootAnalysisManager();
  }
  
  return fgInstance;
}    

//_____________________________________________________________________________
G4RootAnalysisManager::G4RootAnalysisManager()
 : G4VAnalysisManager("Root"),
   fFile(0),
   fHistoDirectory(0),
   fNtupleDirectory(0),
   fH1Vector(),   
   fH2Vector(),   
   fNtuple(0),
   fNtupleBooking(0),
   fNtupleIColumnMap(),
   fNtupleFColumnMap(),
   fNtupleDColumnMap()
{
  if ( fgInstance ) {
    G4ExceptionDescription description;
    description << "      " 
                << "G4RootAnalysisManager already exists." 
                << "Cannot create another instance.";
    G4Exception("G4RootAnalysisManager::G4RootAnalysisManager()",
                "Analysis_F001", FatalException, description);
  }              
   
  fgInstance = this;
}

//_____________________________________________________________________________
G4RootAnalysisManager::~G4RootAnalysisManager()
{  
  std::vector<tools::histo::h1d*>::iterator it;
  for (it = fH1Vector.begin(); it != fH1Vector.end(); it++ ) {
    delete (*it);
  }  
  
  std::vector<tools::histo::h2d*>::iterator it2;
  for (it2 = fH2Vector.begin(); it2 != fH2Vector.end(); it2++ ) {
    delete (*it2);
  }  

  delete fNtuple;
  delete fNtupleBooking;
  delete fFile;  

  fgInstance = 0;
}

// 
// private methods
//

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::CreateHistoDirectory()
{
  if ( fHistoDirectoryName == "" ) {
    // Do not create a new directory if its name is not set
    fHistoDirectory = &(fFile->dir());
    return true;
  }  
  
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "directory for histograms", fHistoDirectoryName);
#endif
  
  fHistoDirectory = fFile->dir().mkdir(fHistoDirectoryName);
  if ( ! fHistoDirectory ) {
    G4ExceptionDescription description;
    description << "      " 
                << "cannot create directory " << fHistoDirectoryName;
    G4Exception("G4RootAnalysisManager::CreateHistoDirectory()",
              "Analysis_W002", JustWarning, description);
    return false;       
  }       
#ifdef G4VERBOSE
  else {
    if ( fpVerboseL1 ) 
      fpVerboseL1->Message("create", "directory for histograms", fHistoDirectoryName);
  }    
#endif
  return true;
}

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::CreateNtupleDirectory()
{
  if ( fNtupleDirectoryName == "" ) {
    // Do not create a new directory if its name is not set
    fNtupleDirectory = &(fFile->dir());
    return true;
  }  
  
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3
      ->Message("create", "directory for ntuples", fNtupleDirectoryName);
#endif

  fNtupleDirectory = fFile->dir().mkdir(fNtupleDirectoryName);
  if ( ! fNtupleDirectory ) {
    G4ExceptionDescription description;
    description << "      " 
                << "cannot create directory " << fNtupleDirectoryName;
    G4Exception("G4RootAnalysisManager::CreateNtupleDirectory()",
                "Analysis_W002", JustWarning, description);
    return false;       
  }       
#ifdef G4VERBOSE
  else {
    if ( fpVerboseL1 ) 
      fpVerboseL1
        ->Message("create", "directory for ntuples", fNtupleDirectoryName);
  }    
#endif
  return true;
}

//_____________________________________________________________________________
void G4RootAnalysisManager::CreateNtupleFromBooking()
{
// Create ntuple from ntuple_booking.

  if ( fNtuple || (! fNtupleBooking) ) return;       

#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create from booking", "ntuple", fNtupleBooking->m_name);
#endif
  fNtuple = new tools::wroot::ntuple(*fNtupleDirectory, *fNtupleBooking);
  
  if ( fNtupleBooking->m_columns.size() ) {
    // store ntuple columns in local maps
    const std::vector<tools::ntuple_booking::col_t>& columns 
      = fNtupleBooking->m_columns;
    std::vector<tools::ntuple_booking::col_t>::const_iterator it;
    G4int index = 0;
    for ( it = columns.begin(); it!=columns.end(); ++it) {
      if ( (*it).second == tools::_cid(int(0) ) ) {
        G4cout << "adding int " << fNtuple->find_column<int>((*it).first) << G4endl;
        fNtupleIColumnMap[index++] = fNtuple->find_column<int>((*it).first);
      }
      else if( (*it).second == tools::_cid(float(0) ) ) {
        fNtupleFColumnMap[index++] = fNtuple->find_column<float>((*it).first);
      } 
      else if((*it).second== tools::_cid(double(0))) {
        fNtupleDColumnMap[index++] = fNtuple->find_column<double>((*it).first);
      }
      else {
        G4ExceptionDescription description;
        description << "      " 
                    << "Unsupported column type " << (*it).first;
        G4Exception("G4RootAnalysisManager::CreateNtupleFromBooking()",
                    "Analysis_W004", JustWarning, description);
      }
    }
  }
}   

//_____________________________________________________________________________
tools::wroot::ntuple::column<int>*    
G4RootAnalysisManager::GetNtupleIColumn(G4int id) const
{
  std::map<G4int, tools::wroot::ntuple::column<int>* >::const_iterator it
    = fNtupleIColumnMap.find(id);
  if ( it == fNtupleIColumnMap.end() ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::GetNtupleIColumn()",
                "Analysis_W009", JustWarning, description);
    return 0;
  }
  
  return it->second;
}  
    
//_____________________________________________________________________________
tools::wroot::ntuple::column<float>*  
G4RootAnalysisManager::GetNtupleFColumn(G4int id) const
{
  std::map<G4int, tools::wroot::ntuple::column<float>* >::const_iterator it
    = fNtupleFColumnMap.find(id);
  if ( it == fNtupleFColumnMap.end() ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::GetNtupleFColumn()",
                "Analysis_W009", JustWarning, description);
    return 0;
  }
  
  return it->second;
}  


//_____________________________________________________________________________
tools::wroot::ntuple::column<double>* 
G4RootAnalysisManager::GetNtupleDColumn(G4int id) const
{
  std::map<G4int, tools::wroot::ntuple::column<double>* >::const_iterator it
    = fNtupleDColumnMap.find(id);
  if ( it == fNtupleDColumnMap.end() ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::GetNtupleDColumn()",
                "Analysis_W009", JustWarning, description);
    return 0;
  }
  
  return it->second;
}  
 
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::Reset()
{
// Reset histograms and ntuple

  G4bool finalResult = true;

  std::vector<tools::histo::h1d*>::iterator it;
  for (it = fH1Vector.begin(); it != fH1Vector.end(); it++ ) {
    G4bool result = (*it)->reset();
    if ( ! result ) finalResult = false;
  }  
  
  std::vector<tools::histo::h2d*>::iterator it2;
  for (it2 = fH2Vector.begin(); it2 != fH2Vector.end(); it2++ ) {
    G4bool result = (*it2)->reset();
    if ( ! result ) finalResult = false;
  }  
  
  // ntuple is deleted automatically when file is closed
  //delete fNtuple;
  fNtuple = 0;
  
  return finalResult;
}  
 
//
// protected methods
//

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::WriteOnAscii(std::ofstream& output)
{
// Write selected objects on ASCII file
// (Only H1 implemented by now)
// According to the implementation by Michel Maire, originally in
// extended examples.

  // h1 histograms
  for ( G4int i=0; i<G4int(fH1Vector.size()); ++i ) {
    G4int id = i + fFirstHistoId;
    G4HnInformation* info = GetH1Information(id); 
    // skip writing if activation is enabled and H1 is inactivated
    if ( ! info->fAscii ) continue; 
    tools::histo::h1d* h1 = fH1Vector[i];

#ifdef G4VERBOSE
    if ( fpVerboseL2 ) 
      fpVerboseL2->Message("write on ascii", "h1d", info->fName);
#endif
  
    output << "\n  1D histogram " << id << ": " << h1->title() 
           << "\n \n \t     X \t\t     Y" << G4endl;
    
    for (G4int i=0; i< G4int(h1->axis().bins()); ++i) {
       output << "  " << i << "\t" 
              << h1->axis().bin_center(i) << "\t"
              << h1->bin_height(i) << G4endl;
    } 
  }
  
  return true;
}  


// 
// public methods
//

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::OpenFile(const G4String& fileName)
{
  // Keep file name
  fFileName =  fileName;

  // Add file extension .root if no extension is given
  G4String name(fileName);
  if ( name.find(".") == std::string::npos ) { 
    name.append(".");
    name.append(GetFileType());
  }  
  
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("open", "analysis file", name);
#endif

  // delete previous file if exists
  if ( fFile ) delete fFile;
  
  fFile = new tools::wroot::file(std::cout, name);
  if ( ! fFile->is_open() ) {
    G4ExceptionDescription description;
    description << "      " << "Cannot open file " << fileName;
    G4Exception("G4RootAnalysisManager::OpenFile()",
                "Analysis_W001", JustWarning, description);
    return false;
  }

  // Create directories
  if ( ! CreateHistoDirectory() ) return false;
  if ( ! CreateNtupleDirectory() ) return false;
  
  // Create ntuple if it is booked
  if ( fNtupleBooking && ( ! fNtuple ) ) 
    CreateNtupleFromBooking();
  
  fLockFileName = true;
  fLockHistoDirectoryName = true;
  fLockNtupleDirectoryName = true;

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("open", "analysis file", name);
#endif
  
  return true;
}  
  
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::Write() 
{
  // h1 histograms
  for ( G4int i=0; i<G4int(fH1Vector.size()); ++i ) {
    G4int id = i + fFirstHistoId;
    G4HnInformation* info = GetH1Information(id); 
    // skip writing if activation is enabled and H1 is inactivated
    if ( fActivation && ( ! info->fActivation ) ) continue; 
    tools::histo::h1d* h1 = fH1Vector[i];
#ifdef G4VERBOSE
    if ( fpVerboseL2 ) 
      fpVerboseL2->Message("write", "h1d", info->fName);
#endif
    G4bool result
      = to(*fHistoDirectory,*h1,info->fName);
    if ( ! result ) {
      G4ExceptionDescription description;
      description << "      " << "saving histo " << info->fName << " failed";
      G4Exception("G4RootAnalysisManager::Write()",
                "Analysis_W003", JustWarning, description);
      return false;       
    } 
  }

  // h2 histograms
  for ( G4int i=0; i<G4int(fH2Vector.size()); ++i ) {
    G4int id = i + fFirstHistoId;
    G4HnInformation* info = GetH2Information(id); 
    // skip writing if inactivated
    if ( fActivation && ( ! info->fActivation ) ) continue;
    tools::histo::h2d* h2 = fH2Vector[i];
#ifdef G4VERBOSE
    if ( fpVerboseL2 ) 
      fpVerboseL2->Message("write", "h2d", info->fName);
#endif
    G4bool result
      = to(*fHistoDirectory,*h2,info->fName);
    if ( ! result ) {
      G4ExceptionDescription description;
      description << "      " << "saving histo " << info->fName << " failed";
      G4Exception("G4RootAnalysisManager::Write()",
                "Analysis_W003", JustWarning, description);
      return false;       
    } 
  }

#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("write", "file", GetFullFileName());
#endif

  unsigned int n;
  G4bool result = fFile->write(n);

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("write", "file", GetFullFileName(), result);
#endif

  // Write ASCII if activated
  if ( IsAscii() ) {
    G4bool result2 = WriteAscii();
    result = result && result2;
  }   

  return result;
}

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::CloseFile()
{
  G4bool result = true;

#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("close", "file", GetFullFileName());
#endif

  // reset data
  result = Reset();
  if ( ! result ) {
      G4ExceptionDescription description;
      description << "      " << "Resetting data failed";
      G4Exception("G4RootAnalysisManager::Write()",
                "Analysis_W002", JustWarning, description);
      result = false;       
  } 

  // close file
  fFile->close();  
  fLockFileName = false;

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("close", "file", GetFullFileName());
#endif

  return result;
} 
   
//_____________________________________________________________________________
G4int G4RootAnalysisManager::CreateH1(const G4String& name,  const G4String& title,
                               G4int nbins, G4double xmin, G4double xmax,
                               G4double unit)
{
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "H1", name);
#endif
  G4int index = fH1Vector.size();
  tools::histo::h1d* h1 = new tools::histo::h1d(title, nbins, xmin, xmax);
            // h1 objects are deleted in destructor and reset when 
            // closing a file.
  fH1Vector.push_back(h1);
  AddH1Information(name, unit);
  
  fLockFirstHistoId = true;
#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "H1", name);
#endif
  return index + fFirstHistoId;
}                                         

//_____________________________________________________________________________
G4int G4RootAnalysisManager::CreateH2(const G4String& name,  const G4String& title,
                               G4int nxbins, G4double xmin, G4double xmax,
                               G4int nybins, G4double ymin, G4double ymax,
                               G4double xunit, G4double yunit)
                               
{
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "H2", name);
#endif
  G4int index = fH2Vector.size();
  tools::histo::h2d* h2 
    = new tools::histo::h2d(title, nxbins, xmin, xmax, nybins, ymin, ymax);
            // h2 objects are deleted in destructor and reset when 
            // closing a file.
  fH2Vector.push_back(h2);
  AddH2Information(name, xunit, yunit);
  
  fLockFirstHistoId = true;
#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "H2", name);
#endif
  return index + fFirstHistoId;
}                                         

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::SetH1(G4int id,
                                G4int nbins, G4double xmin, G4double xmax,
                                G4double unit)
{                                

  tools::histo::h1d* h1d = GetH1(id, false, false);
  if ( ! h1d ) {
    G4ExceptionDescription description;
    description << "      " << "histogram " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::SetH1()",
                "Analysis_W007", JustWarning, description);
    return false;
  }

  G4HnInformation* info = GetH1Information(id);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("configure", "H1", info->fName);
#endif

  h1d->configure(nbins, xmin, xmax);
  info->fXUnit = unit;
  info->fYUnit = unit;
  info->fActivation = true;
  
  return true;
}
  
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::SetH2(G4int id,
                                G4int nxbins, G4double xmin, G4double xmax, 
                                G4int nybins, G4double ymin, G4double ymax,
                                G4double xunit, G4double yunit) 
{                                
  tools::histo::h2d* h2d = GetH2(id, false, false);
  if ( ! h2d ) {
    G4ExceptionDescription description;
    description << "      " << "histogram " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::SetH2()",
                "Analysis_W007", JustWarning, description);
    return false;
  }

  G4HnInformation* info = GetH2Information(id);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("configure", "H2", info->fName);
#endif

  h2d->configure(nxbins, xmin, xmax, nybins, ymin, ymax);
  info->fXUnit = xunit;
  info->fYUnit = yunit;
  info->fActivation = true;
  
  return true;
}
                                  
//_____________________________________________________________________________
void G4RootAnalysisManager::CreateNtuple(const G4String& name, 
                                         const G4String& title)
{
  if ( fNtupleBooking ) {
    G4ExceptionDescription description;
    description << "      " 
                << "Ntuple already exists. "
                << "(Only one ntuple is currently supported.)";
    G4Exception("G4RootAnalysisManager::CreateNtuple()",
                "Analysis_W006", JustWarning, description);
    return;       
  }

  // Create a directory if file is open
  if ( fFile && ( ! fNtupleDirectory ) ) {
    if ( ! CreateNtupleDirectory() ) return;
  }
  
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "ntuple", name);
#endif

  // Create ntuple booking
  fNtupleBooking = new tools::ntuple_booking();
  fNtupleBooking->m_name = name;
  fNtupleBooking->m_title = title;
           // ntuple booking object is deleted in destructor

  // Create ntuple if the file is open
  if ( fFile ) {
    fNtuple = new tools::wroot::ntuple(*fNtupleDirectory, name, title);
           // ntuple object is deleted automatically when closing a file
  }

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "ntuple", name);
#endif
}                                         

//_____________________________________________________________________________
G4int G4RootAnalysisManager::CreateNtupleIColumn(const G4String& name)
{
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "ntuple I column", name);
#endif

  if ( ! fNtupleBooking ) {
    G4ExceptionDescription description;
    description << "      " 
                << "Ntuple has to be created first. ";
    G4Exception("G4RootAnalysisManager::CreateNtupleIColumn()",
                "Analysis_W005", JustWarning, description);
    return -1;       
  }

  // Save column info in booking
  G4int index = fNtupleBooking->m_columns.size();
  fNtupleBooking->add_column<int>(name);  
 
  // Create column if ntuple already exists
  if ( fNtuple ) {
    tools::wroot::ntuple::column<int>* column 
      = fNtuple->create_column<int>(name);  
    fNtupleIColumnMap[index] = column;
  }  

  fLockFirstNtupleColumnId = true;

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "ntuple I column", name);
#endif

  return index + fFirstNtupleColumnId;       
}                                         

//_____________________________________________________________________________
G4int G4RootAnalysisManager::CreateNtupleFColumn(const G4String& name)
{
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "ntuple F column", name);
#endif

  if ( ! fNtupleBooking )  {
    G4ExceptionDescription description;
    description << "      " 
                << "Ntuple has to be created first. ";
    G4Exception("G4RootAnalysisManager::CreateNtupleFColumn()",
                "Analysis_W005", JustWarning, description);
    return -1;       
  }

  // Save column info in booking
  G4int index = fNtupleBooking->m_columns.size();
  fNtupleBooking->add_column<float>(name);  
 
  // Create column if ntuple already exists
  if ( fNtuple ) {
    tools::wroot::ntuple::column<float>* column 
      = fNtuple->create_column<float>(name);  
    fNtupleFColumnMap[index] = column;
  }  

  fLockFirstNtupleColumnId = true;

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "ntuple F column", name);
#endif

  return index + fFirstNtupleColumnId;       
}                                         


//_____________________________________________________________________________
G4int G4RootAnalysisManager::CreateNtupleDColumn(const G4String& name)   
{
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) 
    fpVerboseL3->Message("create", "ntuple D column", name);
#endif

  if ( ! fNtupleBooking ) {
    G4ExceptionDescription description;
    description << "      " 
                << "Ntuple has to be created first. ";
    G4Exception("G4RootAnalysisManager::CreateNtupleDColumn()",
                "Analysis_W005", JustWarning, description);
    return -1;       
  }

  // Save column info in booking
  G4int index = fNtupleBooking->m_columns.size();
  fNtupleBooking->add_column<double>(name);  
 
  // Create column if ntuple already exists
  if ( fNtuple ) {
    tools::wroot::ntuple::column<double>* column 
      = fNtuple->create_column<double>(name);  
    fNtupleDColumnMap[index] = column;
  }  
  
  fLockFirstNtupleColumnId = true;

#ifdef G4VERBOSE
  if ( fpVerboseL1 ) 
    fpVerboseL1->Message("create", "ntuple D column", name);
#endif

  return index + fFirstNtupleColumnId;       
}                                         

//_____________________________________________________________________________
void G4RootAnalysisManager::FinishNtuple()
{ 
  // nothing to be done here
}
   
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::FillH1(G4int id, G4double value, G4double weight)
{
  tools::histo::h1d* h1d = GetH1(id);
  if ( ! h1d ) {
    G4ExceptionDescription description;
    description << "      " << "histogram " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::FillH1()",
                "Analysis_W007", JustWarning, description);
    return false;
  }

  if ( fActivation && ( ! GetActivation(kH1, id) ) ) {
    //G4cout << "Skipping FillH1 for " << id << G4endl; 
    return false; 
  }  

  h1d->fill(value/GetXUnit(kH1, id), weight);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) {
    G4ExceptionDescription description;
    description << " id " << id << " value " << value;
    fpVerboseL3->Message("fill", "H1", description);
  }  
#endif
  return true;
}

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::FillH2(G4int id, 
                                     G4double xvalue, G4double yvalue, 
                                     G4double weight)
{
  tools::histo::h2d* h2d = GetH2(id);
  if ( ! h2d ) {
    G4ExceptionDescription description;
    description << "      " << "histogram " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::FillH2()",
                "Analysis_W007", JustWarning, description);
    return false;
  }

  if ( fActivation && ( ! GetActivation(kH2, id) ) ) return false; 

  h2d->fill(xvalue/GetXUnit(kH2, id), yvalue/GetYUnit(kH2, id), weight);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) {
    G4ExceptionDescription description;
    description << " id " << id 
                << " xvalue " << xvalue << " yvalue " << yvalue;
    fpVerboseL3->Message("fill", "H2", description);
  }  
#endif
  return true;
}

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::FillNtupleIColumn(G4int id, G4int value)
{
  tools::wroot::ntuple::column<int>* column = GetNtupleIColumn(id);
  if ( ! column ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::FillNtupleIColumn()",
                "Analysis_W009", JustWarning, description);
    return false;
  }  
  
  column->fill(value);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) {
    G4ExceptionDescription description;
    description << " id " << id << " value " << value;
    fpVerboseL3->Message("fill", "ntuple I column", description);
  }  
#endif
  return true;       
}                                         
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::FillNtupleFColumn(G4int id, G4float value)
{
  tools::wroot::ntuple::column<float>* column = GetNtupleFColumn(id);
  if ( ! column ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::FillNtupleFColumn()",
                "Analysis_W009", JustWarning, description);
    return false;
  }  
  
  column->fill(value);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) {
    G4ExceptionDescription description;
    description << " id " << id << " value " << value;
    fpVerboseL3->Message("fill", "ntuple F column", description);
  }  
#endif
  return true;       
}                                         
//_____________________________________________________________________________
G4bool G4RootAnalysisManager::FillNtupleDColumn(G4int id, G4double value)
{
  tools::wroot::ntuple::column<double>* column = GetNtupleDColumn(id);
  if ( ! column ) {
    G4ExceptionDescription description;
    description << "      " << "column " << id << " does not exist.";
    G4Exception("G4RootAnalysisManager::FillNtupleDColumn()",
                "Analysis_W009", JustWarning, description);
    return false;
  }  
  
  column->fill(value);
#ifdef G4VERBOSE
  if ( fpVerboseL3 ) {
    G4ExceptionDescription description;
    description << " id " << id << " value " << value;
    fpVerboseL3->Message("fill", "ntuple D column", description);
  }  
#endif
  return true;       
}                                         

//_____________________________________________________________________________
G4bool G4RootAnalysisManager::AddNtupleRow()
{ 
#ifdef G4VERBOSE
  if ( fpVerboseL3 )
    fpVerboseL3->Message("add", "ntuple row", "");
#endif

  if ( ! fNtuple ) {
    G4ExceptionDescription description;
    description << "      " << "ntuple does not exist. ";
    G4Exception("G4RootAnalysisManager::AddNtupleRow()",
                "Analysis_W008", JustWarning, description);
    return false;
  }  
  
  G4bool result =fNtuple->add_row();
  if ( ! result ) {
    G4ExceptionDescription description;
    description << "      " << "adding row has failed.";
    G4Exception("G4RootAnalysisManager::AddNtupleRow()",
                "Analysis_W004", JustWarning, description);
  }         
#ifdef G4VERBOSE
  if ( fpVerboseL3 )
    fpVerboseL3->Message("add", "ntuple row", "", result);
#endif

  return result;
}

//_____________________________________________________________________________
tools::histo::h1d*  G4RootAnalysisManager::GetH1(G4int id, G4bool warn,
                                                 G4bool onlyIfActive) const 
{
  G4int index = id - fFirstHistoId;
  if ( index < 0 || index >= G4int(fH1Vector.size()) ) {
    if ( warn) {
      G4ExceptionDescription description;
      description << "      " << "histo " << id << " does not exist.";
      G4Exception("G4RootAnalysisManager::GetH1()",
                  "Analysis_W007", JustWarning, description);
    }
    return 0;         
  }
  
  // Do not return histogram if inactive 
  if ( fActivation && onlyIfActive && ( ! GetActivation(kH1, id) ) ) {
    return 0; 
  }  
  
  return fH1Vector[index];
}

//_____________________________________________________________________________
tools::histo::h2d*  G4RootAnalysisManager::GetH2(G4int id, G4bool warn,
                                                 G4bool onlyIfActive) const 
{
  G4int index = id - fFirstHistoId;
  if ( index < 0 || index >= G4int(fH2Vector.size()) ) {
    if ( warn) {
      G4ExceptionDescription description;
      description << "      " << "histo " << id << " does not exist.";
      G4Exception("G4RootAnalysisManager::GetH2()",
                  "Analysis_W007", JustWarning, description);
    }
    return 0;         
  }

  // Do not return histogram if inactive 
  if ( fActivation  && onlyIfActive && ( ! GetActivation(kH2, id) ) ) {
    return 0; 
  }  
  
  return fH2Vector[index];
}

//_____________________________________________________________________________
tools::wroot::ntuple* G4RootAnalysisManager::GetNtuple() const
{
  return fNtuple;
}  
