# Track Fitting

Tested and ran on larsoft `v10_06_00_e26_prof`.

+ `AnalysisPandoTrack_module.cc`: Retrieves Pandora reconstructed tracks from LAr output.

+ `workflow_reco2.fcl`: Introduces `pandoraKalmanTrack` module as part of reco2 producer and sequence. Copy to `sbndcode/sbndcode/JobConfigurations/standard/reco/config/`.
+ `pandoramodules_sbnd.fcl`: Contains pandora parameters. Copy to `sbndcode/sbndcode/SBNDPandora/`.