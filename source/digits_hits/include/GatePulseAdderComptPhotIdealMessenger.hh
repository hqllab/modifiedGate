

#ifndef GatePulseAdderComptPhotIdealMessenger_h
#define GatePulseAdderComptPhotIdealMessenger_h 1

#include "GatePulseProcessorMessenger.hh"

class G4UIdirectory;
class G4UIcmdWithoutParameter;
class G4UIcmdWithAString;
class G4UIcmdWithABool;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;

class GatePulseAdderComptPhotIdeal;


class GatePulseAdderComptPhotIdealMessenger: public GatePulseProcessorMessenger
{
  public:
    GatePulseAdderComptPhotIdealMessenger(GatePulseAdderComptPhotIdeal* itsPulseAdder);
    virtual ~GatePulseAdderComptPhotIdealMessenger();

    void SetNewValue(G4UIcommand* aCommand, G4String aString);

    inline GatePulseAdderComptPhotIdeal* GetPulseAdderComptPhotIdeal()
      { return (GatePulseAdderComptPhotIdeal*) GetPulseProcessor(); }
private:
      G4UIcmdWithABool* pRejectionPolicyCmd;

};

#endif
