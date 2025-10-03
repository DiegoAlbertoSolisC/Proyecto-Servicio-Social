#include "LXePrimaryGeneratorAction.hh"
#include "LXePrimaryGeneratorMessenger.hh"
#include "LXeEventAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "Randomize.hh"
#include <cmath>

// Añadir using declaration para twopi
using CLHEP::twopi;

LXePrimaryGeneratorAction::LXePrimaryGeneratorAction()
{
  fEnergyMin = 1.0 * MeV;
  fEnergyMax = 10.0 * MeV;

  fParticleGun = new G4ParticleGun(1);

  auto particleTable = G4ParticleTable::GetParticleTable();
  fParticleGun->SetParticleDefinition(particleTable->FindParticle("gamma"));

  fParticleGun->SetParticleEnergy(511.0 * keV);
  fParticleGun->SetParticlePosition(G4ThreeVector(0.0, 0.0, -20.0 * cm));
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0, 0.0, 1.0));

  fMessenger = new LXePrimaryGeneratorMessenger(this);
}

LXePrimaryGeneratorAction::~LXePrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
}

void LXePrimaryGeneratorAction::GeneratePrimaries(G4Event* evt)
{
  const G4double energy = fEnergyMin + G4UniformRand() * (fEnergyMax - fEnergyMin);
  fParticleGun->SetParticleEnergy(energy);

  const G4double diskRadius = 60.0 * cm;
  const G4double zPos = -40.0 * cm;

  const G4double rho = std::sqrt(G4UniformRand()) * diskRadius;
  const G4double phi = twopi * G4UniformRand(); // Usar twopi directamente
  const G4double xPos = rho * std::cos(phi);
  const G4double yPos = rho * std::sin(phi);

  G4ThreeVector primaryPos(xPos, yPos, zPos);
  fParticleGun->SetParticlePosition(primaryPos);

  const G4UserEventAction* uea = G4RunManager::GetRunManager()->GetUserEventAction();
  const LXeEventAction* evActConst = dynamic_cast<const LXeEventAction*>(uea);
  if (evActConst) {
    auto* evAct = const_cast<LXeEventAction*>(evActConst);
    evAct->SetPrimaryEnergy(energy);
    evAct->SetPrimaryPosition(primaryPos);
  }

  fParticleGun->GeneratePrimaryVertex(evt);
}

void LXePrimaryGeneratorAction::SetEnergyMin(G4double val) { fEnergyMin = val; }
void LXePrimaryGeneratorAction::SetEnergyMax(G4double val) { fEnergyMax = val; }