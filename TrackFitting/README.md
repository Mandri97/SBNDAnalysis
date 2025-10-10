# Track Fitting

Tested and ran on larsoft `v10_06_00_e26_prof`.

+ `AnalysisPandoTrack_module.cc`: Retrieves Pandora reconstructed tracks from LAr output.

+ `workflow_reco2.fcl`: Replaces the track fitting module to `sbnd_pandoraKalmanTrackCreation`.
+ `pandoramodules_sbnd.fcl`: Contains pandora parameters. Copy to `sbndcode/sbndcode/SBNDPandora/`.
