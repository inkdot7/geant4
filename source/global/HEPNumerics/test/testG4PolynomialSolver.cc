#include "G4PolynomialSolver.hh"

class MyFunctionClass 
{
public:
  MyFunctionClass (G4double param)
  {
    Parameter = param;
  }
  
  ~MyFunctionClass () {;}

  setParam(G4double param)
  {
    Parameter = param;    
  }
  

G4double MyFunctionClass::Function(G4double value)
{
  G4double result;

  result = value * value - Parameter;
  
  return result ;  
}    

G4double MyFunctionClass::Derivative(G4double value)
  {
    G4double result;

    result = 2 * value ;
    
    return result;
  }

private:
  G4double Parameter;
} ;



int main (void)
{
  MyFunctionClass MyFunction(1.0);

  G4PolynomialSolver<MyFunctionClass,G4double(MyFunctionClass::*)(G4double)>
    PolySolver(&MyFunction,
	       &MyFunctionClass::Function,
	       &MyFunctionClass::Derivative,
	       1e-6) ;


  G4double val;

  val = PolySolver.solve(0.0,2.0);

  G4cout << "val = " << val << G4endl;

  MyFunction.setParam(3.0);
  
  val = PolySolver.solve(0.0,2.0);

  G4cout << "val = " << val << G4endl;

  
  return 0;  
}



