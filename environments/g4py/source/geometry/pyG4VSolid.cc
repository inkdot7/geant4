// $Id: pyG4VSolid.cc,v 1.2 2006-04-25 08:09:45 kmura Exp $
// $Name: not supported by cvs2svn $
// ====================================================================
//   pyG4VSolid.cc
//
//                                         2005 Q
// ====================================================================
#include <boost/python.hpp>
#include "G4VSolid.hh"

using namespace boost::python;

// ====================================================================
// module definition
// ====================================================================
void export_G4VSolid()
{
  class_<G4VSolid, G4VSolid*, boost::noncopyable> 
    ("G4VSolid", "solid class", no_init)
    // ---
    .def("GetName",    &G4VSolid::GetName)
    .def("SetName",    &G4VSolid::SetName)
    .def("DumpInfo",   &G4VSolid::DumpInfo)
    // operators
    .def(self == self)
    ;
}

