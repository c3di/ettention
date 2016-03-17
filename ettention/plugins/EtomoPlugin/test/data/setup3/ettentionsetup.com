# Command file for running ettentionsetup created by makecomfile
$ettentionsetup -StandardInput
CommandFile	tilt.com
algorithm.numberOfIterations 3
algorithm.lambda	0.3
algorithm	sart
algorithm.useLongObjectCompensation	true
hardware.openclDeviceId	10