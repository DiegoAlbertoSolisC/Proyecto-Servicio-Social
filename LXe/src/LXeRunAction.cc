#include "LXeRunAction.hh"
#include "LXeHistoManager.hh"

#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4AutoLock.hh"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <regex>

namespace fs = std::filesystem;

LXeRunAction::LXeRunAction() = default;
LXeRunAction::~LXeRunAction() = default;

void LXeRunAction::BeginOfRunAction(const G4Run*)
{
  // Asegura que cada hilo tenga su HistoManager
  (void)LXeHistoManager::GetInstance();
}

static bool IsHeader(const std::string& line)
{
  return line.rfind("eventID", 0) == 0;
}

void LXeRunAction::EndOfRunAction(const G4Run*)
{
  // Solo el maestro fusiona y borra
  if (!G4Threading::IsMasterThread()) return;

  const std::string mergedName = "pmt_hits_all.csv";
  std::ofstream merged(mergedName, std::ios::out | std::ios::trunc);
  if (!merged.is_open()) {
    G4cerr << "[MergeCSV] No pude crear " << mergedName << G4endl;
    return;
  }

  std::regex patt(R"(pmt_hits_t\d+\.csv)");
  bool wroteHeader = false;
  std::vector<fs::path> toDelete;

  for (const auto& entry : fs::directory_iterator(fs::current_path())) {
    const std::string fname = entry.path().filename().string();
    if (!std::regex_match(fname, patt)) continue;

    std::ifstream in(entry.path());
    if (!in.is_open()) {
      G4cerr << "[MergeCSV] No pude abrir " << fname << G4endl;
      continue;
    }

    toDelete.push_back(entry.path());

    std::string line;
    bool firstLine = true;
    while (std::getline(in, line)) {
      if (firstLine && IsHeader(line)) {
        firstLine = false;
        if (wroteHeader) continue;   // cabecera ya añadida
        wroteHeader = true;
      }
      merged << line << '\n';
      firstLine = false;
    }
    in.close();
  }
  merged.close();
  G4cout << "[MergeCSV] Fusionado → " << mergedName << G4endl;

  // Elimina intermedios
  for (const auto& p : toDelete) {
    std::error_code ec;
    fs::remove(p, ec);
    if (ec) {
      G4cerr << "[MergeCSV] No se pudo borrar " << p << " : "
             << ec.message() << G4endl;
    }
  }
  G4cout << "[MergeCSV] Eliminados CSV intermedios (" << toDelete.size()
         << " archivos)" << G4endl;
}
