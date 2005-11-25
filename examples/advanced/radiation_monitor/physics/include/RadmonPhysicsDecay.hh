//
// File name:     RadmonPhysicsDecay.hh
// Creation date: Nov 2005
// Main author:   Riccardo Capra <capra@ge.infn.it>
//
// Id:            $Id: RadmonPhysicsDecay.hh,v 1.2 2005-11-25 01:52:01 capra Exp $
// Tag:           $Name: not supported by cvs2svn $
//
// Description:   Decay processes
//

#ifndef   RADMONPHYSICSDECAY_HH
 #define  RADMONPHYSICSDECAY_HH
 
 // Include files
 #include "RadmonVSubPhysicsListWithLabel.hh"
 #include "RadmonPhysicsInfoList.hh"
 
 class RadmonPhysicsDecay : public RadmonVSubPhysicsListWithLabel
 {
  public:
   inline                                       RadmonPhysicsDecay();
   inline virtual                              ~RadmonPhysicsDecay();

   virtual RadmonVSubPhysicsListWithLabel *     New(void) const;

   virtual void                                 ConstructParticle(void);
   virtual void                                 ConstructProcess(void);
   virtual void                                 SetCuts(void);
   
   virtual const RadmonPhysicsInfoList &        Provides(void) const;

  protected:
   
  private:
  // Hidden constructors and operators
                                                RadmonPhysicsDecay(const RadmonPhysicsDecay & copy);
   RadmonPhysicsDecay &                         operator=(const RadmonPhysicsDecay & copy);
   
   mutable RadmonPhysicsInfoList                infoList;
 };
 
 #include "RadmonPhysicsDecay.icc"
#endif /* RADMONPHYSICSDECAY_HH */
