// $Id: G4DCIOentryT.hh,v 1.1 2002-11-24 13:45:23 morita Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// File: G4DCIOentryT.hh
//
// History:
//   '01.09.12  Youhei Morita  Initial creation

#ifndef DCIO_ENTRY_T_HH
#define DCIO_ENTRY_T_HH 1

#include <string>
#include "G4VPDigitsCollectionIO.hh"

// Class inherited:
#include "G4VDCIOentry.hh"

// Class Description:
//   Template class of DigitsCollection I/O Manager for late binding

template <class T> class G4DCIOentryT
 : public G4VDCIOentry
{
    public: // With description
      G4DCIOentryT<T>(std::string n)
       : G4VDCIOentry(n), f_manager(0)
      {
         if ( m_verbose > 2 ) {
           std::cout << "G4DCIOentryT: Registering DigitsCollection IO manager"
                << " for \"" << n << "\"" <<  std::endl;
         }
      }
      // Constructor

      ~G4DCIOentryT() {};
      // Destructor

    public: // With description
      void CreateDCIOmanager(std::string detName, std::string colName)
      {
        if ( f_manager == 0 ) {
          f_manager = new T( detName, colName );
          if ( m_verbose > 2 ) {
            std::cout << "G4DCIOentryT: Constructing DigitsCollection IO manager"
                << " for \"" << detName << "\" " << f_manager <<  std::endl;
          }
          G4DCIOcatalog::GetG4DCIOcatalog()->RegisterDCIOmanager(f_manager);
          if ( m_verbose > 2 ) {
            G4DCIOcatalog::GetG4DCIOcatalog()->PrintDCIOmanager();
          }
        }
      }
      // Create a new digits collection I/O manager

      void DeleteDCIOmanager() { if (f_manager!=0) delete f_manager; };
      // Delete a digits collection I/O manager

    private:
      G4VPDigitsCollectionIO* f_manager;

}; // End of class G4DCIOentryT

#endif

