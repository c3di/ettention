# Command file to run Tilt
#
####CreatedVersion#### 4.0.15
# 
# RADIAL specifies the frequency at which the Gaussian low pass filter begins
#   followed by the standard deviation of the Gaussian roll-off
#
# LOG takes the logarithm of tilt data after adding the given value
#
$tilt -StandardInput
InputProjections jason64.ali
OutputFile jason64.rec
IMAGEBINNED 1
TILTFILE jason64.tlt
THICKNESS 20
RADIAL 0.35 0.05
XAXISTILT 0.0
SCALE 0.0 1000.0
PERPENDICULAR 
MODE 2
FULLIMAGE 64 64
SUBSETSTART 0 0
AdjustOrigin 
ActionIfGPUFails 1,2
OFFSET 0.0
SHIFT 0.0 0.0
LOG 0.0
$if (-e ./savework) ./savework
