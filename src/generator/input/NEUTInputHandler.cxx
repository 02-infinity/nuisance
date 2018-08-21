#include "generator/input/NEUTInputHandler.hxx"

#include "utility/InteractionChannelUtility.hxx"
#include "utility/PDGCodeUtility.hxx"
#include "utility/ROOTUtility.hxx"

#include "generator/utility/NEUTUtility.hxx"

#include "fhiclcpp/ParameterSet.h"

using namespace nuis::event;
using namespace nuis::utility;
using namespace nuis::neuttools;

NEUTInputHandler::NEUTInputHandler() : fInputTree(nullptr) {}
NEUTInputHandler::NEUTInputHandler(NEUTInputHandler &&other)
    : fInputTree(std::move(other.fInputTree)),
      fReaderEvent(std::move(other.fReaderEvent)) {}

void NEUTInputHandler::Initialize(fhicl::ParameterSet const &ps) {

  fInputTree = CheckGetTTree(ps.get<std::string>("file"), "neuttree");

  fReaderEvent.fNeutVect = nullptr;
  fInputTree->tree->SetBranchAddress("vectorbranch", &fReaderEvent.fNeutVect);

  fKeepIntermediates = ps.get<bool>("keep_intermediates", false);
}

MinimalEvent const &NEUTInputHandler::GetMinimalEvent(ev_index_t idx) const {
  if (idx >= GetNEvents()) {
    throw IInputHandler::invalid_entry()
        << "[ERROR]: Attempted to get entry " << idx
        << " from an InputHandler with only " << GetNEvents();
  }
  fInputTree->tree->GetEntry(idx);

  fReaderEvent.mode = IntToChannel(fReaderEvent.fNeutVect->Mode);

  size_t NPart = fReaderEvent.fNeutVect->Npart();
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fReaderEvent.fNeutVect->PartInfo(part_it));
    if ((part.fIsAlive == false) && (part.fStatus == -1) &&
        IsNeutralLepton(part.fPID)) {
      fReaderEvent.probe_E = part.fP.T();
      fReaderEvent.probe_pdg = part.fPID;
      break;
    }
  }

  return fReaderEvent;
}

FullEvent const &NEUTInputHandler::GetFullEvent(ev_index_t idx) const {
  (void)GetMinimalEvent(idx);

  fReaderEvent.ClearParticleStack();

  if (fReaderEvent.fNeutVect->Ibound) {
    fReaderEvent.target_pdg = MakeNuclearPDG(fReaderEvent.fNeutVect->TargetA,
                                             fReaderEvent.fNeutVect->TargetZ);
  } else {
    fReaderEvent.target_pdg = MakeNuclearPDG(1, 1);
  }

  size_t NPart = fReaderEvent.fNeutVect->Npart();
  size_t NPrimary = fReaderEvent.fNeutVect->Nprimary();
  for (size_t part_it = 0; part_it < NPart; part_it++) {
    NeutPart const &part = (*fReaderEvent.fNeutVect->PartInfo(part_it));

    Particle nuis_part;
    nuis_part.pdg = part.fPID;
    nuis_part.P4 = part.fP;

    Particle::Status_t state = GetNeutParticleStatus(part, fReaderEvent.mode);

    if (!fKeepIntermediates && (state == Particle::Status_t::kIntermediate)) {
      continue;
    }

    size_t state_int = static_cast<size_t>(state);

    // Add status == 0 particles as pre-FSI particles until we find an
    // intermediate state particle
    if ((part_it < NPrimary) &&
        (state != Particle::Status_t::kPrimaryInitialState)) {
      fReaderEvent
          .ParticleStack[static_cast<size_t>(
              Particle::Status_t::kPrimaryFinalState)]
          .particles.push_back(nuis_part);
    }

    fReaderEvent.ParticleStack[state_int].particles.push_back(nuis_part);
  }

  return fReaderEvent;
}

size_t NEUTInputHandler::GetNEvents() const {
  return fInputTree->tree->GetEntries();
}

DECLARE_PLUGIN(IInputHandler, NEUTInputHandler);
