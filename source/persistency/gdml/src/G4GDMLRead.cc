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

#include "G4GDMLRead.hh"

G4GDMLRead::G4GDMLRead() {

   try {

      xercesc::XMLPlatformUtils::Initialize();
   }
   catch(xercesc::XMLException& e) {

      char* message = xercesc::XMLString::transcode(e.getMessage());
      G4cerr << "XML toolkit initialization error: " << message << G4endl;
      xercesc::XMLString::release(&message);
   }

   parser = new xercesc::XercesDOMParser;

   parser->setValidationScheme(xercesc::XercesDOMParser::Val_Always);
   parser->setDoNamespaces(true);
   parser->setDoSchema(true);
   parser->setValidationSchemaFullChecking(true);
   parser->setCreateEntityReferenceNodes(false);   // Entities will be automatically resolved by Xerces
}

G4GDMLRead::~G4GDMLRead() {

   if (parser) delete parser;

   try {

      xercesc::XMLPlatformUtils::Terminate();
   }
   catch(xercesc::XMLException& e) {
    
      char* message = xercesc::XMLString::transcode(e.getMessage());
      G4cerr << "XML toolkit termination error: " << message << G4endl;
      xercesc::XMLString::release(&message);
   }
}

G4String G4GDMLRead::GenerateName(const G4String& in) {

   std::string out(prename+"_");
   
   std::string::size_type open = in.find("[",0);

   out.append(in,0,open);
   
   while (open != std::string::npos) {
   
      std::string::size_type close = in.find("]",open);

      if (close == std::string::npos) G4Exception("Bracket mismatch in loop!");
   
      std::string expr = in.substr(open+1,close-open-1);

      std::stringstream stream;
      
      stream << "[" << eval.EvaluateInteger(expr) << "]";
   
      out.append(stream.str());

      open = in.find("[",close);
   }

   return out;
}

void G4GDMLRead::Read(const G4String& fileName) {

   prename = fileName;

   try {

      parser->parse(fileName.c_str());
   }
   catch (const xercesc::XMLException &e) {
   
      char* message = xercesc::XMLString::transcode(e.getMessage());
      G4cout << "XML: " << message << G4endl;
      xercesc::XMLString::release(&message);
   }
   catch (const xercesc::DOMException &e) {
   
      char* message = xercesc::XMLString::transcode(e.getMessage());
      G4cout << "DOM: " << message << G4endl;
      xercesc::XMLString::release(&message);
   }

   xercesc::DOMDocument* doc = parser->getDocument();

   if (!doc) G4Exception("GDML: Unable to open document: "+fileName);

   xercesc::DOMElement* element = doc->getDocumentElement();

   if (!element) G4Exception("GDML: Empty document!");

   for (xercesc::DOMNode* iter = element->getFirstChild();iter != 0;iter = iter->getNextSibling()) {

      if (iter->getNodeType() != xercesc::DOMNode::ELEMENT_NODE) continue;

      const xercesc::DOMElement* const child = dynamic_cast<xercesc::DOMElement*>(iter);

      const G4String tag = xercesc::XMLString::transcode(child->getTagName());

      if (tag=="define") defineRead(child); else
      if (tag=="materials") materialsRead(child); else
      if (tag=="solids") solidsRead(child); else
      if (tag=="setup") setupRead(child); else
      if (tag=="structure") structureRead(child);
   }
}

void G4GDMLRead::loopRead(const xercesc::DOMElement* const element,void(G4GDMLRead::*func)(const xercesc::DOMElement* const)) {

   G4String var;
   G4String from;
   G4String to;
   G4String step;

   const xercesc::DOMNamedNodeMap* const attributes = element->getAttributes();
   XMLSize_t attributeCount = attributes->getLength();

   for (XMLSize_t attribute_index=0;attribute_index<attributeCount;attribute_index++) {

      xercesc::DOMNode* attribute_node = attributes->item(attribute_index);

      if (attribute_node->getNodeType() != xercesc::DOMNode::ATTRIBUTE_NODE) continue;

      const xercesc::DOMAttr* const attribute = dynamic_cast<xercesc::DOMAttr*>(attribute_node);   

      const G4String attribute_name = xercesc::XMLString::transcode(attribute->getName());
      const G4String attribute_value = xercesc::XMLString::transcode(attribute->getValue());

      if (attribute_name=="var") var = attribute_value; else
      if (attribute_name=="from") from = attribute_value; else
      if (attribute_name=="to") to = attribute_value; else
      if (attribute_name=="step") step = attribute_value;
   }

   eval.checkVariable(var);

   G4int _var = eval.EvaluateInteger(var);
   G4int _from = eval.EvaluateInteger(from);
   G4int _to = eval.EvaluateInteger(to);
   G4int _step = eval.EvaluateInteger(step);
   
   if (!from.empty()) _var = _from;

   while (_var <= _to) {
   
      eval.setVariable(var,_var);
      (this->*func)(element);

      _var += _step;
   }
}

G4PVPlacement* G4GDMLRead::getWorldVolume(const G4String& setupName) {

   G4LogicalVolume* volume = getVolume(getSetup(setupName));

   volume->SetVisAttributes(G4VisAttributes::Invisible);

   return new G4PVPlacement(0,G4ThreeVector(),volume,"",0,0,0);
}
