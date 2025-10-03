#ifndef LXePrimaryGeneratorAction_h
#define LXePrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class LXePrimaryGeneratorMessenger;

class LXePrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  LXePrimaryGeneratorAction();
  ~LXePrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

  void SetEnergyMin(G4double val);
  void SetEnergyMax(G4double val);

private:
  G4ParticleGun* fParticleGun = nullptr;          // owned
  LXePrimaryGeneratorMessenger* fMessenger = nullptr;  // owned

  G4double fEnergyMin = 0.;
  G4double fEnergyMax = 0.;
};

#endif
