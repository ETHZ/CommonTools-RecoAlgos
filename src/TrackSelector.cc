#include "CommonTools/RecoAlgos/interface/TrackSelector.h"

using namespace reco;

namespace helper 
{

  TrackCollectionStoreManager::
  TrackCollectionStoreManager(const edm::Handle<reco::TrackCollection> & ) 
    :
    selTracks_( new reco::TrackCollection ),
    selTrackExtras_( new reco::TrackExtraCollection ),
    selHits_( new TrackingRecHitCollection ),
    selStripClusters_( new edmNew::DetSetVector<SiStripCluster> ),
    selPixelClusters_( new edmNew::DetSetVector<SiPixelCluster> ),
    rTracks_(), rTrackExtras_(), rHits_(),
    clusterStorer_(),
    idx_(0), hidx_(0),
    cloneClusters_ (true)
  {
  }
  
  
  //------------------------------------------------------------------
  //!  Process a single track.  
  //------------------------------------------------------------------
  void 
  TrackCollectionStoreManager::
  processTrack( const Track & trk ) 
  {
    selTracks_->push_back( Track( trk ) );
    selTracks_->back().setExtra( TrackExtraRef( rTrackExtras_, idx_ ++ ) );
// Switch to reduced extra (AA)    
//    selTrackExtras_->push_back( TrackExtra( trk.outerPosition(), trk.outerMomentum(), trk.outerOk(),
//					    trk.innerPosition(), trk.innerMomentum(), trk.innerOk(),
//					    trk.outerStateCovariance(), trk.outerDetId(),
//					    trk.innerStateCovariance(), trk.innerDetId(),
//					    trk.seedDirection() ) );
    selTrackExtras_->push_back( TrackExtra( trk.outerOk(),
              trk.innerOk(),
              trk.outerStateCovariance(), trk.outerDetId(),
              trk.innerStateCovariance(), trk.innerDetId(),
              trk.seedDirection() ) );
              
    TrackExtra & tx = selTrackExtras_->back();
    for( trackingRecHit_iterator hit = trk.recHitsBegin(); hit != trk.recHitsEnd();
	 ++ hit, ++ hidx_ ) {

        selHits_->push_back( (*hit)->clone() );
        TrackingRecHit * newHit = & (selHits_->back());
        tx.add( TrackingRecHitRef( rHits_, hidx_ ) );

        //--- Skip the rest for this hit if we don't want to clone the cluster.
        //--- The copy constructer in the rec hit will copy the link properly.
        if (cloneClusters() && newHit->isValid()
	    && ((*hit)->geographicalId().det() == DetId::Tracker)) {
	  clusterStorer_.addCluster( *selHits_, hidx_ );
        }
    } // end of for loop over tracking rec hits on this track
  } // end of track, and function


  //------------------------------------------------------------------
  //!  Put tracks, track extras and hits+clusters into the event.
  //------------------------------------------------------------------
  edm::OrphanHandle<reco::TrackCollection> 
  TrackCollectionStoreManager::
  put( edm::Event & evt ) {
    edm::OrphanHandle<reco::TrackCollection> 
      h = evt.put( selTracks_ );
    evt.put( selTrackExtras_ );
    evt.put( selHits_ );
    evt.put( selStripClusters_ );
    evt.put( selPixelClusters_ );
    return h; 
  }
  
} // end of namespace helper

