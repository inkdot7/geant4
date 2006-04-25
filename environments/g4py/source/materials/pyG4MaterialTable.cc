// $Id: pyG4MaterialTable.cc,v 1.2 2006-04-25 08:09:45 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   pyG4MaterialTable.cc
//
//                                         2005 Q
// ====================================================================
#include <boost/python.hpp>
#include "pyG4indexing.hh"
#include "G4Material.hh"
#include "G4MaterialTable.hh"

using namespace boost::python;

// ====================================================================
// module definition
// ====================================================================
void export_G4MaterialTable()
{
  class_<G4MaterialTable> ("G4MaterialTable", "material table")
    .def(vector_indexing_suite<G4MaterialTable>())
    .def(self_ns::str(self))
    ;
}

