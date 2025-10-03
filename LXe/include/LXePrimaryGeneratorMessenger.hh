#ifndef LXePrimaryGeneratorMessenger_h
#define LXePrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithADoubleAndUnit;
class LXePrimaryGeneratorAction;

class LXePrimaryGeneratorMessenger : public G4UImessenger {
public:
  LXePrimaryGeneratorMessenger(LXePrimaryGeneratorAction* action);
  virtual ~LXePrimaryGeneratorMessenger();

  virtual void SetNewValue(G4UIcommand*, G4String);

private:
  LXePrimaryGeneratorAction* fAction;

  G4UIdirectory* fGunDir;
  G4UIcmdWithADoubleAndUnit* fEnergyMinCmd;
  G4UIcmdWithADoubleAndUnit* fEnergyMaxCmd;
};

#endif
