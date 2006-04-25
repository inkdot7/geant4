// $Id: pymodG4graphics_reps.cc,v 1.2 2006-04-25 08:09:45 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   pymodG4graphics_reps.cc [Geant4Py module]
//
//                                         2005 Q
// ====================================================================
#include <boost/python.hpp>

using namespace boost::python;

// ====================================================================
// module definition
// ====================================================================

void export_G4VisAttributes();
void export_G4Colour();

BOOST_PYTHON_MODULE(G4graphics_reps) 
{
  export_G4VisAttributes();
  export_G4Colour();
}

