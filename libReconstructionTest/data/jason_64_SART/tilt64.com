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
OutputFile ../../work/jason64/generated.mrc
TILTFILE jason64.tlt
RADIAL 0.35 0.05
XAXISTILT -1.6
SCALE 0.0 0.1
PERPENDICULAR 
MODE 1
SUBSETSTART 0 0
AdjustOrigin 
ActionIfGPUFails 1,2
OFFSET 0.0
SHIFT 0.0 0.0
UseGPU 0
$if (-e ./savework) ./savework
