# Command file for running ettentionsetup created by makecomfile
$ettentionsetup -StandardInput
CommandFile	tilt.com
algorithm.numberOfIterations 3
algorithm.lambda	0.3
algorithm	blockIterative
algorithm.useLongObjectCompensation	true
forwardProjection.samples 9
backProjection.samples	4
volume.dimension 32.0,64.0,10.0
volume.origin	-20.0,0.0,0.0
algorithm.blockSize 6