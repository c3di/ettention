# Command file for running ettentionsetup created by makecomfile
$ettentionsetup -StandardInput
algorithm	sart
algorithm.numberOfIterations 3
algorithm.lambda	0.3
algorithm.useLongObjectCompensation	true
volume.origin	-50.0,70.0,0.0
volume.dimension 200.0,300.0,40.0
forwardProjection.samples 9
backProjection.samples	4