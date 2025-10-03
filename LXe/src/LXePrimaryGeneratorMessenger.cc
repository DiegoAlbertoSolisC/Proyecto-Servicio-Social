#include "LXePrimaryGeneratorMessenger.hh"
#include "LXePrimaryGeneratorAction.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIdirectory.hh"

LXePrimaryGeneratorMessenger::LXePrimaryGeneratorMessenger(LXePrimaryGeneratorAction* act)
: fAction(act)
{
  fGunDir = new G4UIdirectory("/LXe/gun/");
  fGunDir->SetGuidance("Controls for the primary generator");

  fEnergyMinCmd = new G4UIcmdWithADoubleAndUnit("/LXe/gun/energyMin", this);
  fEnergyMinCmd->SetGuidance("Set minimum energy");
  fEnergyMinCmd->SetUnitCategory("Energy");
  fEnergyMinCmd->SetParameterName("Emin", false);
  fEnergyMinCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fEnergyMaxCmd = new G4UIcmdWithADoubleAndUnit("/LXe/gun/energyMax", this);
  fEnergyMaxCmd->SetGuidance("Set maximum energy");
  fEnergyMaxCmd->SetUnitCategory("Energy");
  fEnergyMaxCmd->SetParameterName("Emax", false);
  fEnergyMaxCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

LXePrimaryGeneratorMessenger::~LXePrimaryGeneratorMessenger()
{
  delete fEnergyMinCmd;
  delete fEnergyMaxCmd;
  delete fGunDir;
}

void LXePrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fEnergyMinCmd)
    fAction->SetEnergyMin(fEnergyMinCmd->GetNewDoubleValue(newValue));
  else if (command == fEnergyMaxCmd)
    fAction->SetEnergyMax(fEnergyMaxCmd->GetNewDoubleValue(newValue));
}
