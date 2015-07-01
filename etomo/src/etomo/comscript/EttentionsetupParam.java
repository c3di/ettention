package etomo.comscript;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import etomo.BaseManager;
import etomo.comscript.SirtsetupParam.Field;
import etomo.storage.LogFile;
import etomo.type.AxisID;
import etomo.type.ConstEtomoNumber;
import etomo.type.ConstIntKeyList;
import etomo.type.EtomoBoolean2;
import etomo.type.EtomoNumber;
import etomo.type.FileType;
import etomo.type.IteratorElementList;
import etomo.type.ProcessName;
import etomo.type.ScriptParameter;
import etomo.type.StringParameter;

/**
* <p>Description: </p>
* 
* <p>Copyright: Copyright 2011</p>
*
* <p>Organization:
* Boulder Laboratory for 3-Dimensional Electron Microscopy of Cells (BL3DEMC),
* University of Colorado</p>
* 
* @author $Author$
* 
* @version $Revision$
* 
* <p> $Log$
* <p> Revision 1.2  2011/05/03 02:43:13  sueh
* <p> but# 1416 Implemented CommandDetails.
* <p>
* <p> Revision 1.1  2011/04/04 16:47:34  sueh
* <p> bug# 1416 Param class for sirtsetup.
* <p> </p>
*/
public final class EttentionsetupParam implements CommandParam, CommandDetails {
  public static final String rcsid = "$Id$";

  public static final String NUMBER_OF_ITERATIONS_KEY = "algorithm.numberOfIterations";
  public static final String LAMBDA_KEY = "algorithm.lambda";

  public static final String NUMBER_OF_PROJECTIONS_KEY = "algorithm.blockSize";
  
  public static final String OVERSAMPLING_FORWARD_PROJECTION_KEY = "forwardProjection.samples";
  public static final String OVERSAMPLING_BACK_PROJECTION_KEY = "backProjection.samples";
  
  
  private final ScriptParameter numberOfIterations = new ScriptParameter(
		  EtomoNumber.Type.INTEGER, NUMBER_OF_ITERATIONS_KEY);
  private final ScriptParameter lambda = new ScriptParameter(
	      EtomoNumber.Type.DOUBLE, LAMBDA_KEY);
  private final ScriptParameter numberOfProjections = new ScriptParameter(
		  EtomoNumber.Type.INTEGER, NUMBER_OF_PROJECTIONS_KEY);

  private final ScriptParameter oversamplingForwardProjection = new ScriptParameter(
		  EtomoNumber.Type.INTEGER, OVERSAMPLING_FORWARD_PROJECTION_KEY);

  private final ScriptParameter oversamplingBackProjection = new ScriptParameter(
		  EtomoNumber.Type.INTEGER, OVERSAMPLING_BACK_PROJECTION_KEY);
  
 
 
  private final AxisID axisID;
  private final BaseManager manager;
  
 // private final StringParameter commandFile = new StringParameter("CommandFile");
  
  private final StringParameter reconstructionType = new StringParameter("algorithm");
  private final StringParameter useLongObjectCompensation = new StringParameter("algorithm.useLongObjectCompensation");
  private final StringParameter subareaSize = new StringParameter("volume.dimension");  
  private final StringParameter subareaCenter = new StringParameter("volume.origin");

  public LinkedList<StringParameter> additionalParameter = new LinkedList<StringParameter>();
  
  public EttentionsetupParam(final BaseManager manager, final AxisID axisID) {
    this.axisID = axisID;
    this.manager = manager;
 //   commandFile.set(FileType.TILT_COMSCRIPT.getFileName(manager, axisID));
    reconstructionType.set("sart");
    numberOfIterations.set(1);
    lambda.set(0.3);
    subareaCenter.set("0.0,0.0,0.0");
  }

  public void parseComScriptCommand(final ComScriptCommand scriptCommand)
      throws BadComScriptException, InvalidParameterException,
      FortranInputSyntaxException {
	  // reset
	  numberOfIterations.reset();
	  lambda.reset();
	  reconstructionType.reset();
	  numberOfProjections.reset();
	  oversamplingForwardProjection.reset();
	  oversamplingBackProjection.reset();
	  useLongObjectCompensation.reset();
	  subareaSize.reset();
	  subareaCenter.reset();
	  // parse
	  numberOfIterations.parse(scriptCommand);
	  lambda.parse(scriptCommand);
	  reconstructionType.parse(scriptCommand);
	  numberOfProjections.parse(scriptCommand);
	  oversamplingForwardProjection.parse(scriptCommand);
	  oversamplingBackProjection.parse(scriptCommand);
	  useLongObjectCompensation.parse(scriptCommand);
	  subareaSize.parse(scriptCommand);
	  subareaCenter.parse(scriptCommand);
  }

  public void updateComScriptCommand(final ComScriptCommand scriptCommand)
      throws BadComScriptException {
    scriptCommand.useKeywordValue();
 //   commandFile.updateComScript(scriptCommand);
    reconstructionType.updateComScript(scriptCommand);
    numberOfIterations.updateComScript(scriptCommand);
    lambda.updateComScript(scriptCommand);
    numberOfProjections.updateComScript(scriptCommand);
    oversamplingForwardProjection.updateComScript(scriptCommand);
    oversamplingBackProjection.updateComScript(scriptCommand);
    useLongObjectCompensation.updateComScript(scriptCommand);
	subareaSize.updateComScript(scriptCommand);
	subareaCenter.updateComScript(scriptCommand);
	
	Iterator<StringParameter> iter = additionalParameter.iterator();
	while ( iter.hasNext() )
	{
		StringParameter parameter = iter.next();
		parameter.updateComScript(scriptCommand);		
	}
  }

  public void initializeDefaults() {
  }

  public boolean isOversamplingForwardProjectionNull() {
	    return oversamplingForwardProjection.isNull();
  }

  public void setReconstructionType(final String input) {
	  reconstructionType.set(input);
  }
  
  public void setNumberOfProjections(final String input) {
	  numberOfProjections.set(input);
  }
  
  public void setNumberOfIterations(final String input) {
	  numberOfIterations.set(input);
  }

  public void setLambda(final String input) {
	    lambda.set(input);
  }
  
  public void setOversamplingForwardProjection(final String input){
	  oversamplingForwardProjection.set(input);
  }

  public void setOversamplingBackProjection(final String input){
	  oversamplingBackProjection.set(input);
  }
  
  public void setSubareaSize(final String input) {
	  subareaSize.set(input);
  }
    
  public void setSubareaCenter(final String input) {
	  subareaCenter.set(input);
  }

  public void resetSubareaSize() {
	  subareaSize.reset();
  }
  
  public void resetSubareaCenter() {
	  subareaCenter.reset();
  }
  
  /*public boolean isUseLongObjectCompensation() {
	    return !useLongObjectCompensation.isNull();
  }*/

  public void setUseLongObjectCompensation(boolean input) {
	    if (input) {
	      useLongObjectCompensation.set("true");
	    }
	    else {
	    	useLongObjectCompensation.set("false");
	    }
 }
  
  public String getLambda() {
	return lambda.toString();
  }
	  
  public String getNumberOfIterations() {
		return numberOfIterations.toString();
  }
  
  public String getNumberOfProjections() {
		return numberOfProjections.toString();
 }
  
  public String getReconstructionType(){
	  return reconstructionType.toString();  
  }
  
  public String getOversamplingForwardProjection(){
	  return oversamplingForwardProjection.toString();  
  }
  
  public String getOversamplingBackProjection(){
	  return oversamplingBackProjection.toString();  
  }
  
  public String getUseLongObjectCompensation(){
	  return useLongObjectCompensation.toString();
  }
  
  public String getSubareaSizeX() {
	  return subareaSize.toString();
  }
    
  public String getSubareaCenter() {
	  return subareaCenter.toString();
  }
  
  public void resetOversamplingForwardProjection() {
	  oversamplingForwardProjection.reset();
  }
  
  public void resetOversamplingBackProjection() {
	  oversamplingBackProjection.reset();
  }
  
  public void resetNumberOfProjections() {
	  numberOfProjections.reset();
  }
  
  public void resetUseLongObjectCompensation(){
	  useLongObjectCompensation.reset();
  }
  
  public AxisID getAxisID() {
    return axisID;
  }

  public String getCommand() {
    return ProcessName.ETTENTIONSETUP.getComscript(axisID);
  }

  public String[] getCommandArray() {
    return ProcessName.ETTENTIONSETUP.getComscriptArray(axisID);
  }

  public File getCommandInputFile() {
    return FileType.TILT_COMSCRIPT.getFile(manager, axisID);
  }

  public String getCommandLine() {
    return getCommand();
  }

  public CommandMode getCommandMode() {
    return null;
  }

  public String getCommandName() {
    // In this case the .com file name and the command in the .com file are the same.
    return ProcessName.ETTENTIONSETUP.toString();
  }

  public File getCommandOutputFile() {
    return null;
  }

  public FileType getOutputImageFileType() {
    return null;
  }

  public FileType getOutputImageFileType2() {
    return null;
  }

  public ProcessName getProcessName() {
    return ProcessName.ETTENTIONSETUP;
  }

  public CommandDetails getSubcommandDetails() {
    return null;
  }

  public String getSubcommandProcessName() {
    return null;
  }

  public boolean isMessageReporter() {
    return false;
  }

  public List getLogMessage() throws LogFile.LockException, FileNotFoundException,
      IOException {
    return null;
  }

  public String getName() {
    return ProcessName.ETTENTIONSETUP.toString();
  }


  public void setNumberOfProcessors(final String input) {
	   // numberOfProcessors.set(input);
	  }
  
  public double getDoubleValue(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }

  public ConstEtomoNumber getEtomoNumber(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }

  public Hashtable getHashtable(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }
  
  public boolean getBooleanValue(final etomo.comscript.FieldInterface fieldInterface) {
	//    if (fieldInterface == Field.SUBAREA) {
	//      return !subareaSize.isNull();
	//    }
	    throw new IllegalArgumentException("field=" + fieldInterface);
  }

  public ConstIntKeyList getIntKeyList(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }

  public int getIntValue(final FieldInterface field) {
 //   if (field == Field.Y_OFFSET_OF_SUBSET) {
 //     return yOffsetOfSubarea.getInt();
 //   }
    throw new IllegalArgumentException("field=" + field);
  }

  public IteratorElementList getIteratorElementList(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }

  public String getString(final FieldInterface field) {
 //   if (field == Field.SUBAREA_SIZE) {
  //    return subareaSize.toString(true);
 //   }
    throw new IllegalArgumentException("field=" + field);
  }

  public String[] getStringArray(final FieldInterface field) {
    throw new IllegalArgumentException("field=" + field);
  }

}