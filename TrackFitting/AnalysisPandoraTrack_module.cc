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

#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"

#include "art_root_io/TFileService.h"

#include "lardataobj/RecoBase/Track.h"
#include "lardataobj/RecoBase/TrackingTypes.h"
#include "lardataobj/RecoBase/SpacePoint.h"
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

  float                                  fTrackLength;
  recob::tracking::Point_t               fTrackStart;
  recob::tracking::Point_t               fTrackEnd;
  recob::tracking::Vector_t              fStartDir;
  recob::tracking::Vector_t              fEndDir;
  std::vector<recob::tracking::Point_t>  fTrackPoints;
  std::vector<recob::tracking::Vector_t> fDirections;
  std::vector<recob::tracking::Point_t>  fSpacePoints;

  // input labels
  std::string fSliceLabel;
  std::string fPFParticleLabel;
  std::string fTrackLabel;
  std::string fSPLabel;
};


Analysis::AnalysisPandoraTrack::AnalysisPandoraTrack( fhicl::ParameterSet const& p )
  : EDAnalyzer{ p },
  fSliceLabel{ p.get<std::string>("SliceLabel") },
  fPFParticleLabel{ p.get<std::string>("PFParticleLabel") },
  fTrackLabel{ p.get<std::string>("TrackLabel") },
  fSPLabel{ p.get<std::string>("SpacePointLabel") }
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
  
  art::ValidHandle< std::vector<recob::SpacePoint> > spHandle = 
    e.getValidHandle< std::vector<recob::SpacePoint> >( fSPLabel );

  // Association between particle and track
  art::FindManyP<recob::Track> pfpTrackAssoc( pfpHandle, e, fTrackLabel );
  art::FindManyP<recob::SpacePoint> pfpSPAssoc( pfpHandle, e, fSPLabel );

  for( const art::Ptr<recob::PFParticle> &pfp : pfpVector )
  {
    std::vector< art::Ptr<recob::Track> > tracks = pfpTrackAssoc.at( pfp.key() );
    std::vector< art::Ptr<recob::SpacePoint> > sps = pfpSPAssoc.at( pfp.key() );

    if( tracks.size() != 1 )
    {
      continue;
    }

    const art::Ptr< recob::Track > track = tracks.at( 0 );

    fTrackPoints.clear();
    fSpacePoints.clear();

    fTrackLength = track->Length();
    fTrackStart  = track->Start();
    fTrackEnd    = track->End();
    fStartDir    = track->StartDirection();
    fEndDir      = track->EndDirection();

    size_t n = track->NumberTrajectoryPoints();

    for( size_t i = 0; i < n; ++i )
    {
      fTrackPoints.push_back( track->TrajectoryPoint(i).position );
      fDirections.push_back( track->DirectionAtPoint(i) );
    }

    n = sps.size();

    if( n == 0 ) continue;

    for( size_t i = 0; i < n; ++i ) fSpacePoints.push_back( sps.at(i)->position() );

    fTree->Fill();
  }
}

void Analysis::AnalysisPandoraTrack::beginJob()
{
  // Implementation of optional member function here.
  art::ServiceHandle<art::TFileService> tfs;

  fTree = tfs->make<TTree>("tree", "output TTree");

  fTree->Branch( "eventID",       &fEventID     );
  fTree->Branch( "runID",         &fRunID       );
  fTree->Branch( "subRunID",      &fSubRunID    );
  fTree->Branch( "trackLength",   &fTrackLength );
  fTree->Branch( "trackStart",    &fTrackStart  );
  fTree->Branch( "trackEnd",      &fTrackEnd    );
  fTree->Branch( "startDir",      &fStartDir    );
  fTree->Branch( "endDir",        &fEndDir      );
  fTree->Branch( "trackPoints",   &fTrackPoints );
  fTree->Branch( "spacePoints",   &fSpacePoints  );
  fTree->Branch( "Directions",    &fDirections  );
}

void Analysis::AnalysisPandoraTrack::endJob()
{
  // Implementation of optional member function here.
}

DEFINE_ART_MODULE(Analysis::AnalysisPandoraTrack)
