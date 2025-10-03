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
// * By using,  copying,  modifying or  distributing the software (or *x
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file optical/LXe/src/LXeMainVolume.cc
/// \brief Implementation of the LXeMainVolume class
//
//
#include "LXeMainVolume.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4MaterialPropertiesTable.hh"
#include "G4OpticalSurface.hh"
#include "G4Sphere.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LXeMainVolume::LXeMainVolume(G4RotationMatrix* pRot, const G4ThreeVector& tlate,
                             G4LogicalVolume* pMotherLogical, G4bool pMany, G4int pCopyNo,
                             LXeDetectorConstruction* c)
  // Pass info to the G4PVPlacement constructor
  : G4PVPlacement(
      pRot, tlate,
      // Temp logical volume must be created here
      new G4LogicalVolume(new G4Box("temp", 1, 1, 1), G4Material::GetMaterial("Vacuum"), "temp"),
      "housing", pMotherLogical, pMany, pCopyNo),
    fConstructor(c)
{
  CopyValues();

  //G4double housing_x = fScint_x + 2. * fD_mtl;
  //G4double housing_y = fScint_y + 2. * fD_mtl;
  G4double housing_x = 68 * cm; 
  G4double housing_y = 68 * cm;
  G4double housing_z = 44 * cm;

  //*************************** housing and scintillator
  //fScint_box = new G4Box("scint_box", fScint_x / 2., fScint_y / 2., fScint_z / 2.);
  //fHousing_box = new G4Box("housing_box", housing_x / 2., housing_y / 2., housing_z / 2.);

  //fScint_log = new G4LogicalVolume(fScint_box, G4Material::GetMaterial("LXe"), "scint_log");
  //fHousing_log = new G4LogicalVolume(fHousing_box, G4Material::GetMaterial("Al"), "housing_log");

  //new G4PVPlacement(nullptr, G4ThreeVector(), fScint_log, "scintillator", fHousing_log, false, 0);
  // Reemplazar la caja por un cilindro para el scintilador (aerogel)
  fScint_radius = 60 * cm;  // Radio del cilindro
  fScint_height = fScint_z;       // Altura del cilindro (en el eje Z)
  fScint_box = new G4Tubs("scint_tube", 0., fScint_radius, fScint_height / 2., 0., 2 * M_PI);  // Cilindro con radio y altura definidos

  // Crear el cilindro de housing (si es necesario cambiar también el housing, también usa G4Tubs)
  fHousing_radius = 80 * cm;  // Radio del housing (suponiendo que es un cilindro también)
  fHousing_height = housing_z;       // Altura del housing (en el eje Z)
  fHousing_box = new G4Tubs("housing_tube", 0., fHousing_radius, fHousing_height / 2., 0., 2 * M_PI);  // Cilindro para housing

  // Crear volúmenes lógicos para los cilindros
  // Dentro de la clase LXeMainVolume

  fScint_log = new G4LogicalVolume(fScint_box, G4Material::GetMaterial("Aerogel"), "scint_log");
  fHousing_log = new G4LogicalVolume(fHousing_box, G4Material::GetMaterial("Vacuum"), "housing_log");

  // Colocar los cilindros en el espacio físico
  new G4PVPlacement(nullptr, G4ThreeVector(0,0,-21*cm), fScint_log, "scintillator", fHousing_log, false, 0);

  //*************** Miscellaneous sphere to demonstrate skin surfaces
  fSphere = new G4Sphere("sphere", 0., 2. * cm, 0. * deg, 360. * deg, 0. * deg, 360. * deg);
  fSphere_log = new G4LogicalVolume(fSphere, G4Material::GetMaterial("Al"), "sphere_log");
  //if (fSphereOn)
    //new G4PVPlacement(nullptr, G4ThreeVector(5. * cm, 5. * cm, 5. * cm), fSphere_log, "sphere",
      //                fScint_log, false, 0);

  //****************** Build PMTs
  // Definir dimensiones para el detector cuadrado
  //G4double dx_pmt = fD_mtl * 4.;  // Ancho del detector cuadrado
  //G4double dy_pmt = fD_mtl * 4.;  // Alto del detector cuadrado
  //G4double dz_pmt = fD_mtl * 4.;  // Profundidad del detector cuadrado (puedes ajustarlo según lo necesites)

  G4double dx_pmt = 34 * mm;  // Ancho del detector cuadrado
  G4double dy_pmt = 34 * mm;  // Alto del detector cuadrado
  G4double dz_pmt = fD_mtl * 4.;  // Profundidad del detector cuadrado (puedes ajustarlo según lo necesites)

  // Cambia las declaraciones para usar G4Box en lugar de G4Tubs (lo puse yo)
  //G4Box* fPmt;        // Usar G4Box en lugar de G4Tubs
  //G4Box* fPhotocath;  // Usar G4Box en lugar de G4Tubs


  // Crear la geometría del PMT (detector cuadrado)
  fPmt = new G4Box("pmt_box", dx_pmt, dy_pmt, dz_pmt);

  // Crear la geometría del photocathode (similar, cuadrada)
  fPhotocath = new G4Box("photocath_box", dx_pmt, dy_pmt, dz_pmt / 2.);

  // Crear los volúmenes lógicos para los detectores
  fPmt_log = new G4LogicalVolume(fPmt, G4Material::GetMaterial("Glass"), "pmt_log");
  fPhotocath_log = new G4LogicalVolume(fPhotocath, G4Material::GetMaterial("Al"), "photocath_log");

  // Colocar el photocathode en el PMT (ajustado para un detector cuadrado)
  new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -dz_pmt / 2.), fPhotocath_log, "photocath",
                    fPmt_log, false, 0);

  //***********Arrange pmts around the outside of housing**********

  // Definir los pasos de la cuadrícula para colocar los detectores
  //G4double dx = fScint_x / fNx;  // Ancho de cada detector en la cuadrícula
  //G4double dy = fScint_y / fNy;  // Alto de cada detector en la cuadrícula
  //G4double dz = fScint_z / fNz;  // Profundidad de cada detector en la cuadrícula

  //G4double x, y, z;
  //G4double xmin = -fScint_x / 2. - dx / 2.;  // Limite mínimo en x
  //G4double ymin = -fScint_y / 2. - dy / 2.;  // Limite mínimo en y
  //G4double zmin = -fScint_z / 2. - dz / 2.;  // Limite mínimo en z
  //G4int k = 0;

  // Colocar los detectores en la geometría (distribuidos en una cuadrícula)
//  for (G4int ix = 0; ix < fNx; ix++) {
//      for (G4int iy = 0; iy < fNy; iy++) {
//          for (G4int iz = 0; iz < fNz; iz++) {
//              x = xmin + ix * dx;  // Calcula la posición en x
//              y = ymin + iy * dy;  // Calcula la posición en y
//              z = zmin + iz * dz;  // Calcula la posición en z

              // Colocar cada PMT (detector cuadrado) en la geometría
//               G4int k = 0;
//              new G4PVPlacement(nullptr, G4ThreeVector(x, y, z), fPmt_log, "pmt", fScint_log, false, k++);
//          }
//      }
//  }

 //Esto es lo que agregue
//for (G4int ix = 0; ix < fNx; ix++) {
//  for (G4int iy = 0; iy < fNy; iy++) {
//      for (G4int iz = 0; iz < fNz; iz++) {
            // Calcula las posiciones de los PMTs dentro de los límites
//          G4double x = xmin + ix * dx;
//          G4double y = ymin + iy * dy;
//          G4double z = zmin + iz * dz;

            // Asegúrate de que la posición esté dentro del volumen madre
//          if (std::abs(x) <= fScint_x / 2. && std::abs(y) <= fScint_y / 2. && std::abs(z) <= fScint_z / 2.) {
//              // Colocar el PMT
//              new G4PVPlacement(nullptr, G4ThreeVector(x, y, z), fPmt_log, "pmt", fScint_log, false, 0);
//          }
//      }
//  }
//}

  //G4double innerRadius_pmt = 0.;
  G4double height_pmt = fD_mtl / 2.;
  //G4double startAngle_pmt = 0.;
  //G4double spanningAngle_pmt = 360. * deg;

  //fPmt = new G4Tubs("pmt_tube", innerRadius_pmt, fOuterRadius_pmt, height_pmt, startAngle_pmt,
  //                 spanningAngle_pmt);

  // the "photocathode" is a metal slab at the back of the glass that
  // is only a very rough approximation of the real thing since it only
  // absorbs or detects the photons based on the efficiency set below
  //fPhotocath = new G4Tubs("photocath_tube", innerRadius_pmt, fOuterRadius_pmt, height_pmt / 2.,
  //                      startAngle_pmt, spanningAngle_pmt);

  //fPmt_log = new G4LogicalVolume(fPmt, G4Material::GetMaterial("Glass"), "pmt_log");
  //fPhotocath_log = new G4LogicalVolume(fPhotocath, G4Material::GetMaterial("Al"), "photocath_log");

  //new G4PVPlacement(nullptr, G4ThreeVector(0., 0., -height_pmt / 2.), fPhotocath_log, "photocath",
  //                  fPmt_log, false, 0);

  //***********Arrange pmts around the outside of housing**********

  //G4double dx = fScint_x / fNx;
  //G4double dy = fScint_y / fNy;
  //G4double dx = 5 * cm;
  //G4double dy = 5 * cm;
  
  G4double dz = fScint_z / fNz;


  G4double dx = (2. * fScint_radius - 2 * dx_pmt) / fNx;
  G4double dy = (2. * fScint_radius - 2 * dy_pmt) / fNy;

  G4double xmin = -fNx / 2. * dx + dx / 2.;
  G4double ymin = -fNy / 2. * dy + dy / 2.;

  G4double x, y, z;
  //G4double xmin = -fScint_x / 2. - dx / 2.;
  //G4double ymin = -fScint_y / 2. - dy / 2.;
  //G4double xmin = -34 * cm;
  //G4double ymin = -34 * cm;  

  G4double zmin = -fScint_z / 2. - dz / 2.;
  G4int k = 0;

  //z = -fScint_z / 360. - height_pmt;  // front
  //PlacePMTs(fPmt_log, nullptr, x, y, dx, dy, xmin, ymin, fNx, fNy, x, y, z, k);

  //z = -fScint_z / 2. - height_pmt;  // front
  //PlacePMTs(fPmt_log, nullptr, x, y, dx, dy, xmin, ymin, fNx, fNy, x, y, z, k);
  // Crear la matriz de rotación para colocar los PMTs correctamente (rotación de 180 grados sobre Y)
  //auto rm_z = new G4RotationMatrix();
  //rm_z->rotateY(180. * deg);

  // Ajuste de la posición z para colocar el detector correctamente
  //z = fScint_z / 2. + height_pmt;
  //z = fScint_z / 2. + dz_pmt;  // Ubicar el PMT hacia atrás en z
  // Llamar a la función que coloca los PMTs, asegurándose de pasar las coordenadas correctas
  //PlacePMTs(fPmt_log, rm_z, x, y, dx_pmt, dy_pmt, xmin, ymin, fNx, fNy, x, y, z, k);

  auto rm_z = new G4RotationMatrix();
  rm_z->rotateY(180. * deg);
  //z = fScint_z / 2. + height_pmt;  // back
  z = 21 * cm;
  PlacePMTs(fPmt_log, rm_z, x, y, dx, dy, xmin, ymin, fNx, fNy, x, y, z, k);

  //auto rm_y1 = new G4RotationMatrix();
  //rm_y1->rotateY(-90. * deg);
  //x = -fScint_x / 2. - height_pmt;  // left
  //PlacePMTs(fPmt_log, rm_y1, y, z, dy, dz, ymin, zmin, fNy, fNz, x, y, z, k);

  //auto rm_y2 = new G4RotationMatrix();
  //rm_y2->rotateY(90. * deg);
  //x = fScint_x / 2. + height_pmt;  // right
  //PlacePMTs(fPmt_log, rm_y2, y, z, dy, dz, ymin, zmin, fNy, fNz, x, y, z, k);

  //auto rm_x1 = new G4RotationMatrix();
  //rm_x1->rotateX(90. * deg);
  //y = -fScint_y / 2. - height_pmt;  // bottom
  //PlacePMTs(fPmt_log, rm_x1, x, z, dx, dz, xmin, zmin, fNx, fNz, x, y, z, k);

  //auto rm_x2 = new G4RotationMatrix();
  //rm_x2->rotateX(-90. * deg);
  //y = fScint_y / 2. + height_pmt;  // top
  //PlacePMTs(fPmt_log, rm_x2, x, z, dx, dz, xmin, zmin, fNx, fNz, x, y, z, k);

  VisAttributes();
  SurfaceProperties();

  SetLogicalVolume(fHousing_log);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::CopyValues()
{
  fScint_x = fConstructor->GetScintX();
  fScint_y = fConstructor->GetScintY();
  fScint_z = fConstructor->GetScintZ();
  fD_mtl = fConstructor->GetHousingThickness();
  fNx = fConstructor->GetNX();
  fNy = fConstructor->GetNY();
  fNz = fConstructor->GetNZ();
  fOuterRadius_pmt = fConstructor->GetPMTRadius();
  //fSphereOn = fConstructor->GetSphereOn();
  fRefl = fConstructor->GetHousingReflectivity();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Función modificada
//
void LXeMainVolume::PlacePMTs(G4LogicalVolume* pmt_log, G4RotationMatrix* rot,
                               G4double& a, G4double& b,
                               G4double da, G4double db,
                               G4double amin, G4double bmin,
                               G4int na, G4int nb,
                               G4double& x, G4double& y, G4double& z,
                               G4int& k)
{
  G4double radius = 66.82 * cm;
  G4double spacing = .5 * cm;

  for (G4double xx = -radius; xx <= radius; xx += spacing) {
    for (G4double yy = -radius; yy <= radius; yy += spacing) {
      if (xx*xx + yy*yy <= radius*radius) {
        x = xx;
        y = yy;
        //z = -fScint_z / 2. - 0.5 * fD_mtl;
        z = 21. * cm;


        new G4PVPlacement(nullptr, G4ThreeVector(x, y, z), pmt_log, "pmt", fHousing_log, false, k);
        fPmtPositions.push_back(G4ThreeVector(x, y, z));
        ++k;
      }
    }
  }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::VisAttributes()
{
  //auto housing_va = new G4VisAttributes(G4Colour(0.8, 0.8, 0.8));
  //fHousing_log->SetVisAttributes(housing_va);

  auto sphere_va = new G4VisAttributes();
  sphere_va->SetForceSolid(true);
  fSphere_log->SetVisAttributes(sphere_va);

  auto pmt_va = new G4VisAttributes(G4Colour(3, 2, 4));
  fPmt_log->SetVisAttributes(pmt_va);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LXeMainVolume::SurfaceProperties()
{
  std::vector<G4double> ephoton = {7.0 * eV, 7.14 * eV};

  //**Scintillator housing properties
  std::vector<G4double> reflectivity = {fRefl, fRefl};
  std::vector<G4double> efficiency = {0.0, 0.0};
  auto scintHsngPT = new G4MaterialPropertiesTable();
  scintHsngPT->AddProperty("REFLECTIVITY", ephoton, reflectivity);
  scintHsngPT->AddProperty("EFFICIENCY", ephoton, efficiency);
  //auto OpScintHousingSurface =
  //  new G4OpticalSurface("HousingSurface", unified, polished, dielectric_metal);
  //OpScintHousingSurface->SetMaterialPropertiesTable(scintHsngPT);

  //**Sphere surface properties
  //std::vector<G4double> sphereReflectivity = {1.0, 1.0};
  //std::vector<G4double> sphereEfficiency = {0.0, 0.0};
  //auto spherePT = new G4MaterialPropertiesTable();
  //spherePT->AddProperty("REFLECTIVITY", ephoton, sphereReflectivity);
  //spherePT->AddProperty("EFFICIENCY", ephoton, sphereEfficiency);
  //auto OpSphereSurface = new G4OpticalSurface("SphereSurface", unified, polished, dielectric_metal);
  //OpSphereSurface->SetMaterialPropertiesTable(spherePT);

  //**Photocathode surface properties
  std::vector<G4double> photocath_EFF = {1., 1.};
  std::vector<G4double> photocath_ReR = {1.92, 1.92};
  std::vector<G4double> photocath_ImR = {1.69, 1.69};
  auto photocath_mt = new G4MaterialPropertiesTable();
  photocath_mt->AddProperty("EFFICIENCY", ephoton, photocath_EFF);
  photocath_mt->AddProperty("REALRINDEX", ephoton, photocath_ReR);
  photocath_mt->AddProperty("IMAGINARYRINDEX", ephoton, photocath_ImR);
  auto photocath_opsurf =
    new G4OpticalSurface("photocath_opsurf", glisur, polished, dielectric_metal);
  photocath_opsurf->SetMaterialPropertiesTable(photocath_mt);

  //**Create logical skin surfaces
  //new G4LogicalSkinSurface("photocath_surf", fHousing_log, OpScintHousingSurface);
  //new G4LogicalSkinSurface("sphere_surface", fSphere_log, OpSphereSurface);
  new G4LogicalSkinSurface("photocath_surf", fPhotocath_log, photocath_opsurf);
}
