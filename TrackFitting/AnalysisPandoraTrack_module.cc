////////////////////////////////////////////////////////////////////////
// Class:       AnalysisPandoraTrack
// Plugin Type: analyzer (Unknown Unknown)
// File:        AnalysisPandoraTrack_module.cc
//
// Generated at Wed Jul  2 11:12:54 2025 by Manoa Andriamirado using cetskelgen
// from cetlib version 3.18.02.
////////////////////////////////////////////////////////////////////////

#include "art/Framework/Core/EDAnalyzer.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Principal/Run.h"
#include "art/Framework/Principal/SubRun.h"
#include "canvas/Utilities/InputTag.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include "art_root_io/TFileService.h"

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/TrackingTypes.h"
#include "lardataobj/RecoBase/Slice.h"
#include "lardataobj/RecoBase/PFParticle.h"
#include "canvas/Persistency/Common/FindManyP.h"

#include <TTree.h>

namespace Analysis {
  class AnalysisPandoraTrack;
}


class Analysis::AnalysisPandoraTrack : public art::EDAnalyzer {
public:
  explicit AnalysisPandoraTrack( fhicl::ParameterSet const& p );
  // The compiler-generated destructor is fine for non-base
  // classes without bare pointers or other resource use.

  // Plugins should not be copied or assigned.
  AnalysisPandoraTrack( AnalysisPandoraTrack const& ) = delete;
  AnalysisPandoraTrack( AnalysisPandoraTrack&& ) = delete;
  AnalysisPandoraTrack& operator=( AnalysisPandoraTrack const& ) = delete;
  AnalysisPandoraTrack& operator=( AnalysisPandoraTrack&& ) = delete;

  // Required functions.
  void analyze( art::Event const& e ) override;

  // Selected optional functions.
  void beginJob() override;
  void endJob() override;

private:

  TTree *fTree;

  // Tree variables
  unsigned int fEventID;
  unsigned int fRunID;
  unsigned int fSubRunID;
  int          fPDG;

  float                                 fTrackLength;
  recob::tracking::Point_t              fTrackStart;
  recob::tracking::Point_t              fTrackEnd;
  recob::tracking::Vector_t             fDirection;
  std::vector<recob::tracking::Point_t> fTrackPoints;

  // input labels
  std::string fSliceLabel;
  std::string fPFParticleLabel;
  std::string fTrackLabel;
};


Analysis::AnalysisPandoraTrack::AnalysisPandoraTrack( fhicl::ParameterSet const& p )
  : EDAnalyzer{ p },
  fSliceLabel{ p.get<std::string>("SliceLabel") },
  fPFParticleLabel{ p.get<std::string>("PFParticleLabel") },
  fTrackLabel{ p.get<std::string>("TrackLabel") }
{
  // Call appropriate consumes<>() for any products to be retrieved by this module.
}

void Analysis::AnalysisPandoraTrack::analyze(art::Event const& e)
{
  // Implementation of required member function here.

  fEventID  = e.id().event();
  fRunID    = e.id().run();
  fSubRunID = e.id().subRun();

  // Pfp handle
  art::ValidHandle< std::vector<recob::PFParticle> > pfpHandle =
    e.getValidHandle< std::vector<recob::PFParticle> >( fPFParticleLabel );

  std::vector< art::Ptr<recob::PFParticle> > pfpVector;

  if( pfpHandle.isValid() )
  {
    art::fill_ptr_vector(pfpVector, pfpHandle);
  }

  art::ValidHandle< std::vector<recob::Track> > trackHandle = 
    e.getValidHandle< std::vector<recob::Track> >( fTrackLabel );

  // Association between particle and track
  art::FindManyP<recob::Track> pfpTrackAssoc( pfpHandle, e, fTrackLabel );

  for( const art::Ptr<recob::PFParticle> &pfp : pfpVector )
  {
    fPDG = pfp->PdgCode();
    
    std::vector< art::Ptr<recob::Track> > tracks = pfpTrackAssoc.at( pfp.key() );

    if( tracks.size() != 1 )
    {
      continue;
    }

    art::Ptr< recob::Track > track = tracks.at( 0 );

    fTrackPoints.clear();

    fTrackLength = track->Length();
    fTrackStart  = track->Start();
    fTrackEnd    = track->End();
    fDirection   = track->EndDirection();

    size_t n = track->NumberTrajectoryPoints();

    for( size_t i = 0; i < n; ++i )
    {
      fTrackPoints.push_back( track->TrajectoryPoint(i).position );
    }

    fTree->Fill();
  }
}

void Analysis::AnalysisPandoraTrack::beginJob()
{
  // Implementation of optional member function here.
  art::ServiceHandle<art::TFileService> tfs;

  fTree = tfs->make<TTree>("tree", "output TTree");

  fTree->Branch( "eventID",        &fEventID );
  fTree->Branch( "runID",          &fRunID );
  fTree->Branch( "subRunID",       &fSubRunID );
  fTree->Branch( "PDG",		         &fPDG );
  fTree->Branch( "trackLength",    &fTrackLength );
  fTree->Branch( "trackStart",     &fTrackStart );
  fTree->Branch( "trackEnd",       &fTrackEnd );
  fTree->Branch( "trackDirection", &fDirection );
  fTree->Branch( "trackPoints",    &fTrackPoints );
}

void Analysis::AnalysisPandoraTrack::endJob()
{
  // Implementation of optional member function here.
}

DEFINE_ART_MODULE(Analysis::AnalysisPandoraTrack)
